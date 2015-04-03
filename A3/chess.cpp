// Implementation of chess class method
//
#include "chess.h"

Chess::Chess(const RGB &amb, const RGB &dif, const RGB &spe, 
  const float &shine, const float &refl, const float &transp, 
  const float &transm, const std::string &str, 
  const Vector displacement, const float s, int opt) : 
  mat_ambient(amb), mat_diffuse(dif), mat_specular(spe), 
  mat_shineness(shine), mat_reflection(refl), mat_transparency(transp),
  mat_transmission(transm), optimize(opt)
{
  std::vector<Point > vertices;
  int vertices_cnt = 0, triangles_cnt = 0, line_cnt =0;

  float xmin = INFINITY, ymin = INFINITY, zmin = INFINITY;
  float xmax = -INFINITY, ymax = -INFINITY, zmax = -INFINITY;

  std::ifstream infile(str.c_str());
  std::cout << "reading file:" << str << "..." << std::endl;
  std::string line;

  Mat4 transfer  = Mat4().setTranslation(displacement);
  Mat4 scale     = Mat4().scale(s, s, s);

  while (std::getline(infile, line)){
    line_cnt++;

    std::istringstream buffer(line);
    char type;
    // read the first character and see which type of line it is
    buffer >> type;
    
    if(type == '#') continue;

    if(type == 'v')
    {
      vertices_cnt ++;
      float x, y, z;
      buffer >> x >> y >> z;
      Point p = (scale * transfer).multVecMatrix(Point(x, y, z));

      // std::cout << p << std::endl;

      if(xmin > p.x) xmin = p.x - 0.1;
      if(ymin > p.y) ymin = p.y - 0.1;
      if(zmin > p.z) zmin = p.z - 0.1;
      if(xmax < p.x) xmax = p.x + 0.1;
      if(ymax < p.y) ymax = p.y + 0.1;
      if(zmax < p.z) zmax = p.z + 0.1;

      // std::cout << p << std::endl;
      vertices.push_back(p);
    }
    else if( type == 'f')
    {
      triangles_cnt++;
      int i, j, k;
      buffer >> i >> j >> k;

      // std::cout << i  << "," << j << "," << k << std::endl; 
      
      primitives.push_back(Triangle(vertices[i-1], vertices[j-1], vertices[k-1]));
    }
  }

  if( optimize == 1 || optimize == 2)
  // if( optimize == 1 )
  {
    // points for the 
    Point pMin(xmin, ymin, zmin); 
    Point pMax(xmax, ymax, zmax);

    std::cout << "Bounding Volume " << std::endl;

    // optimization 1 structure
    box = Box(pMin, pMax);

    if( optimize == 2 )
    {
      // calculate the size of grid 
      Vector size = Point(xmax, ymax, zmax) - Point(xmin, ymin, zmin);

      // compute grid number according to a metric proposer by Cleary in 1983
      float root = powf(1 * primitives.size() / (size.x * size.y * size.z), 1 / 3.f);

      for (uint32_t i = 0; i < 3; ++i)
      {
        resolution[i] = std::floor(size[i] * root);
        resolution[i] = std::max(uint32_t(1), std::min(resolution[i], uint32_t(128)));
      }

      // compute the dimensionality of cells
      cellDimension = size / Vector(resolution[0], resolution[1], resolution[2]);

      for (unsigned int i = 0; i < primitives.size(); ++i)
      {
        Point minPoint = primitives[i].get_min_bound();
        Point maxPoint = primitives[i].get_max_bound();

        // calculate the coordination of triangles in the grid
        minPoint = (minPoint - box[0]) / cellDimension;
        maxPoint = (maxPoint - box[0]) / cellDimension;

        uint32_t zmin = clamp<uint32_t>(std::floor(minPoint.z), 0, resolution[2] - 1);
        uint32_t zmax = clamp<uint32_t>(std::floor(maxPoint.z), 0, resolution[2] - 1);
        uint32_t ymin = clamp<uint32_t>(std::floor(minPoint.y), 0, resolution[1] - 1);
        uint32_t ymax = clamp<uint32_t>(std::floor(maxPoint.y), 0, resolution[1] - 1);
        uint32_t xmin = clamp<uint32_t>(std::floor(minPoint.x), 0, resolution[0] - 1);
        uint32_t xmax = clamp<uint32_t>(std::floor(maxPoint.x), 0, resolution[0] - 1);

        // insert the triangle into the corresponding grid
        for (uint32_t z = zmin; z <= zmax; ++z) {
          for (uint32_t y = ymin; y <= ymax; ++y) {
            for (uint32_t x = xmin; x <= xmax; ++x) {
              uint32_t index = z * resolution[0] * resolution[1] + y * resolution[0] + x;
              cells[index].insert(primitives[i]);
            }
          }
        }
        // optimization 2 ended
      }
      // in grid optimization, no need for primitives any more

      // for (std::map<uint32_t, Cell>::iterator it = cells.begin(); it != cells.end(); ++it)
      // {
      //   std::cout << "key: " << it->first << std::endl; 
      // }
    }

  }

  std::cout << "Read file complete: " << line_cnt << " lines in total" << std::endl;
  std::cout << "total points number: "    << vertices_cnt << std::endl;
  std::cout << "total triangles number: " << triangles_cnt << std::endl;
  std::cout << "vertices number: " << vertices.size() << std::endl;
  std::cout << "primitives number: " << primitives.size() << std::endl;
}

bool Chess::intersect(const Ray &ray, Intersection & insect)
{
  Intersection tHit(INFINITY);

  if( optimize == 1){
    // first optimization: a bounding box of the object
    Intersection tmpHit(INFINITY);
    if( !box.intersect(ray, tmpHit) )
      return false;
  }
  else if( optimize == 2)
  {
    // first optimization: a bounding box of the object
    Intersection tmpHit(INFINITY);
    if( !box.intersect(ray, tmpHit) )
      return false;

    Vec3<uint32_t> exit_point, step, cell;
    Vector deltaT, nextCrossingT;
    
    for (uint8_t i = 0; i < 3; ++i) {
      // convert ray starting point to cell coordinates
      float rayOrigCell = ((ray.origin[i] + ray.direction[i] * ray.tmin) -  box[0][i]);
      cell[i] = clamp<uint32_t>(std::floor(rayOrigCell / cellDimension[i]), 0, resolution[i] - 1);
      if (ray.direction[i] < 0) {
        deltaT[i] = -cellDimension[i] * ray.invdir[i];
        nextCrossingT[i] = ray.tnear + (cell[i] * cellDimension[i] - rayOrigCell) * ray.invdir[i];
        exit_point[i] = -1;
        step[i] = -1;
      }
      else {
        deltaT[i] = cellDimension[i] * ray.invdir[i];
        nextCrossingT[i] = ray.tnear + ((cell[i] + 1)  * cellDimension[i] - rayOrigCell) * ray.invdir[i];
        exit_point[i] = resolution[i];
        step[i] = 1;
      }
    }

    // grids traversal
    Intersection tHit(INFINITY);
    while ( true ) {
      // compute cell index for intersection
      uint32_t index = cell[2] * resolution[0] * resolution[1] + cell[1] * resolution[0] + cell[0];
          
      Intersection tmpHit(INFINITY);

      if ( cells.find(index) != cells.end() && cells[index].intersect(ray, tmpHit) ) 
      {  
        if( tmpHit.t < tHit.t )
        {
          tHit = tmpHit;
        }
      }

      uint8_t k = 
        ((nextCrossingT[0] < nextCrossingT[1]) << 2) +
        ((nextCrossingT[0] < nextCrossingT[2]) << 1) +
        ((nextCrossingT[1] < nextCrossingT[2]));
      
      static const uint8_t map[8] = {2, 1, 2, 1, 2, 2, 0, 0};
      
      uint8_t axis = map[k];

      if (ray.tmax < nextCrossingT[axis]) break;
      cell[axis] += step[axis];
      if (cell[axis] == exit_point[axis]) break;
      nextCrossingT[axis] += deltaT[axis];
    }

    if( tHit.t > ray.tmax || tHit.t < ray.tmin ){
      return false;
    }
    else
    {
      insect = tHit;
      return true;
    }
  
  }



  // intersect all primitives 
  for (unsigned int i = 0; i < primitives.size(); ++i)
  {
    Intersection tmpHit(INFINITY);
    if( primitives[i].intersect(ray, tmpHit) )
    {
      if( tmpHit.t < tHit.t )
      {
        tHit = tmpHit;
      }
    }
  }
  
  if( tHit.t > ray.tmax || tHit.t < ray.tmin )
    return false;
  else
  {
    insect = tHit;
    return true;
  }
}

// function for finding intersected triangles within the cell
bool Cell::intersect(const Ray &ray, Intersection & insect)
{
  Intersection tHit(INFINITY);

  // iterate over all triangles inside cell
  for (unsigned int i = 0; i < triangles.size(); ++i) {
    Intersection tmpHit(INFINITY);
    // no mailbox assists in this inplementaton

    if (triangles[i].intersect(ray, tmpHit)) {
      if (tmpHit.t < tHit.t) {
        tHit = tmpHit;
      }
    }
  }

  if( tHit.t > ray.tmax || tHit.t < ray.tmin )
    return false;
  else
  {
    insect = tHit;
    return true;
  }
}
