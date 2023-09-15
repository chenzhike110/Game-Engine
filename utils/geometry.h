#ifndef _CGE_GEOMETRY_H_
#define _CGE_GEOMETRY_H_

#include <vector>
#include <Eigen/Dense>

namespace CGE_UTIL
{

struct Line
{
    Eigen::Vector3d a;
    Eigen::Vector3d b;
    float color[4];
    float lineWidth;
    
    Line(Eigen::Vector3d start, Eigen::Vector3d end): a(start), b(end){}
};

struct triangle
{
    Eigen::Vector3d a;
    Eigen::Vector3d b;
    Eigen::Vector3d c;
    float color[4];

    triangle(Eigen::Vector3d a, Eigen::Vector3d b, Eigen::Vector3d c):a(a), b(b), c(c){}
};

class IndexedTriangleMesh
{
    public:
        IndexedTriangleMesh(std::vector<Eigen::Vector3d> points, std::vector<Eigen::Vector3i> faces): _points(points), _faces(faces) {}
        

    
    private:
        std::vector<Eigen::Vector3d> _points;
        std::vector<Eigen::Vector3i> _faces;



}

}


#endif