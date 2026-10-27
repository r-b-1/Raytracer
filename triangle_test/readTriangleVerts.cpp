#include <iostream>
#include <fstream>
#include <vector>

void readFloatsFromFile(const std::string& filename, std::vector<float> &allFloats)
{
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Failed to open file\n";
        return;
    }

    float value;
    char c;

    while (file >> value) {
        allFloats.push_back(value);

        // Consume comma if there
        if (file.peek() == ',') file.get(c);
    }
}

int main(int argc, char* argv[])
{
    std::vector<float> allFloats;
    readFloatsFromFile( "trilist.dat", allFloats );
    
    std::cout << "Number of floats: " << allFloats.size() << std::endl;
    int numVertices = allFloats.size() / 3;
    std::cout << "Interpreted as Vertices: " << numVertices << std::endl;
    int numTriangles = numVertices / 3;
    std::cout << "Interpreted as Triangles: " << numTriangles << std::endl;
}
