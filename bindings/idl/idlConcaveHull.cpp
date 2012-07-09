#include <stdio.h>
#include "idl_export.h"

#include <iostream>

#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/surface/concave_hull.h>

#include <liblas/liblas.hpp>

#include <geopcl/io/LAStoPCD.hpp>
#include <geopcl/io/PCDtoLAS.hpp>

int
idlConcaveHullnatural(IDL_STRING *input, IDL_STRING *output)
{
  std::cout << "Reading " << IDL_STRING_STR(input) << " and writing " << IDL_STRING_STR(output) << std::endl;

  pcl::PointCloud<pcl::PointXYZI>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZI>);

  liblas::Header header;
  geopcl::LAStoPCD(IDL_STRING_STR(input), header, *cloud);

  // Create the concave hull
  pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_hull(new pcl::PointCloud<pcl::PointXYZI>);
  pcl::ConcaveHull<pcl::PointXYZI> chull;
  chull.setInputCloud(cloud);
  chull.setAlpha(0.1);
  chull.reconstruct(*cloud_hull);

  std::cerr << "Concave hull has: " << cloud_hull->points.size()
            << " data points." << std::endl;

  geopcl::PCDtoLAS(IDL_STRING_STR(output), header, *cloud_hull);

  return 0;
}

int
idlConcaveHull(int argc, void *argv[])
{
  if (argc != 2)
  {
    std::cerr << "Required arguments: input.las output.las" << std::endl;
    return 1;
  }

  return idlConcaveHullnatural((IDL_STRING *) argv[0], (IDL_STRING *) argv[1]);
}

