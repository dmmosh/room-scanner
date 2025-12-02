#include <fstream>
#include <vector>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <chrono> // For seeding with current time


struct vec3 {
    float x, y, z;
};


struct triangle {
    vec3 normal;
    vec3 v1, v2, v3;
};
std::vector<triangle> tris;

std::ostream& operator<<(std::ostream& os, const vec3& a){
    os << a.x << '\t' << a.y << '\t' << a.z;
    return os;
}

// Compute normal of triangle (right-hand rule)
vec3 computeNormal(const vec3& a, const vec3& b, const vec3& c) {
    vec3 u = { b.x - a.x, b.y - a.y, b.z - a.z };
    vec3 v = { c.x - a.x, c.y - a.y, c.z - a.z };

    vec3 n = {
        u.y * v.z - u.z * v.y,
        u.z * v.x - u.x * v.z,
        u.x * v.y - u.y * v.x
    };
    
    float x = std::sqrt(n.x*n.x + n.y*n.y + n.z*n.z);
    return { n.x/x, n.y/x, n.z/x };
}

// Write binary STL file
void writeBinarySTL(const std::string& filename, const std::vector<triangle>& tris) {
    std::ofstream out(filename, std::ios::binary);
    if (!out) {
        throw std::runtime_error("Unable to open STL file for writing");
    }

    char header[80] = {};
    out.write(header, 80);

    uint32_t triCount = tris.size();
    out.write(reinterpret_cast<char*>(&triCount), sizeof(uint32_t));

    for (const auto& t : tris) {
        out.write(reinterpret_cast<const char*>(&t.normal), sizeof(vec3));
        out.write(reinterpret_cast<const char*>(&t.v1), sizeof(vec3));
        out.write(reinterpret_cast<const char*>(&t.v2), sizeof(vec3));
        out.write(reinterpret_cast<const char*>(&t.v3), sizeof(vec3));

        uint16_t abc = 0; // attribute byte count
        out.write(reinterpret_cast<char*>(&abc), sizeof(uint16_t));
    }
}


inline void reset(){
    tris.clear();
}

inline void add(vec3 a, vec3 b, vec3 c){
    vec3 norm = computeNormal(a,b,c);
    std::cout << norm << '\n' << a << '\n' << b << '\n' << c << '\n' << '\n';
    
    tris.push_back(((triangle){
        normal:{0,0,0},
        v1:a,
        v2:b,
        v3:c
    }));
}

void triangle_test(){
    reset();
    vec3 v0 = {0,0,0};
    vec3 v1 = {10,0,0};
    vec3 v2 = {0,10,0};
    vec3 v3 = {10,10,0};
    vec3 v4 = {5,5,10};

    
    

    add(v0,v1,v3);
    add(v0,v2,v3);


    add(v0,v1,v4);
    add(v0,v2,v4);
    add(v2,v3,v4);
    add(v3,v1,v4);
    writeBinarySTL("triangle_test.stl", tris);
    reset();
};


void jagged_test(const int num, int test_amt){
    test_amt %= 10;
    reset();
    for(int j = 0; j<test_amt; j++){
        vec3 v0 = {10,-15,0};
        vec3 v1= {10,-10,0};
        vec3 v2 = {(v0.x+v1.x)/2,(v0.y+v1.y)/2,-10};
        add(v0,v1,v2);
        v0 = v1;
        for(int i =0; i<num; i++){ // y increasing
            v1 = {r_new(v1.x, 0),r_new(v1.y,1),0};
            v2 = {(v0.x+v1.x)/2,(v0.y+v1.y)/2,-10};
            add(v0,v1,v2);
            v0 = v1;
        }
    
        for(int i =0; i<num; i++){ // x decreasing
            v1 = {r_new(v1.x, -1),r_new(v1.y,0),0};
            v2 = {(v0.x+v1.x)/2,(v0.y+v1.y)/2,-10};
            add(v0,v1,v2);
            v0 = v1;
        }
    
        for(int i =0; i<num; i++){ // y decreasing
            v1 = {r_new(v1.x, 0),r_new(v1.y,-1),0};
            v2 = {(v0.x+v1.x)/2,(v0.y+v1.y)/2,-10};
            add(v0,v1,v2);
            v0 = v1;
        }
    
        for(int i =0; i<num; i++){ // x increasing
            v1 = {r_new(v1.x, 1),r_new(v1.y,0),0};
            v2 = {(v0.x+v1.x)/2,(v0.y+v1.y)/2,-10};
            add(v0,v1,v2);
            v0 = v1;
        }
    
        v1= {10,-10,0};
        v2 = {(v0.x+v1.x)/2,(v0.y+v1.y)/2,-10};
        add(v0,v1,v2);
    
        char filename[] = "string_test_0.stl";
        filename[12] = j + '0';
        writeBinarySTL(filename, tris);
        reset();
        }

}

float r(float min, float max) //range : [min, max]
{
   static bool first = true;
   if (first) 
   {  
      srand( time(NULL) ); //seeding for the first time only!
      first = false;
   }
   return (float)(min + rand() % (int)(( max + 1 ) - min));
}

// 1 increase, 0 not change, -1 decrease
float r_new(const float self, int state){
    switch(state){
    case 1: // increase
        return r(self+1, self+2);
    break;
    case -1: // decrease 
        return r(self-2,self-1);
    break;
    }
    return r(self-5,self+5);
}

int main() {
    jagged_test(100,5);

    triangle_test();

    
}