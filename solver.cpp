#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
using namespace std;


const int W = 1200;
const int H = 900;
const int CHANNEL_NUM = 3;
const string IMG_EXTENSION = ".ppm";
const int IMG_NAME_LENGTH = 4;
const int MAX_VALUE = 255;


void readImage(ifstream& file, vector<vector<vector<int>>>& image)
{
    int h, w, r, g, b;
    string line;
    // scan through header
    for (int i = 0; i < 3; i++)
    {
        if (i == 1)
        {
            //          read image dimensions from second header line
            file >> w >> h;
            image.resize(h, vector<vector<int>>(w, vector<int>(CHANNEL_NUM)));
        }
        getline(file, line);
    }

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            file >> r >> g >> b;
            image[y][x][0] = r;
            image[y][x][1] = g;
            image[y][x][2] = b;
        }
    }
}


void writeImage(const vector<vector<vector<int>>>& image, const string& output_path)
{
    ofstream out(output_path);
    int h = image.size();
    int w = image[0].size();
    // write header
    out << "P3" << endl;
    out << w << " " << h << endl;
    out << MAX_VALUE << endl;
    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            out << image[y][x][0] << " ";
            out << image[y][x][1] << " ";
            out << image[y][x][2] << " ";
        }
    }
}


int main(int argc, char* argv[])
{
    vector<vector<vector<int>>> image;
    string tile_dir = argv[1];
    vector<vector<vector<vector<int>>>> tiles;
    int tile_ind = 0;
    while (true)
    {
        string str_tile_ind = std::to_string(tile_ind);
        int lead_zeros_num = 4 - str_tile_ind.length();
        str_tile_ind.insert(0, lead_zeros_num, '0');
        str_tile_ind += IMG_EXTENSION;
        string image_path = tile_dir + "/" + str_tile_ind;
        ifstream file(image_path.c_str());
        if (!file.is_open())
            break;
        vector<vector<vector<int>>> cur_tile;
        readImage(file, cur_tile);
        tiles.push_back(cur_tile);
        tile_ind += 1;
    }

    // find minimal h and w
    int min_h = tiles[0].size();
    int min_w = tiles[0][0].size();

    for (int i = 1; i < tiles.size(); i++)
    {
        int cur_h = tiles[i].size();
        min_h = min(min_h, cur_h);
        int cur_w = tiles[i][0].size();
        min_w = min(min_w, cur_w);
    }

    // generate nodes
    vector<vector<vector<int>>> resultImage(H, vector<vector<int>>(W, vector<int>(CHANNEL_NUM)));
    vector<pair<int, int>> nodes;
    for (int x = 0; x < W; x += min_w)
    {
        for (int y = 0; y < H; y += min_h)
        {
            nodes.push_back({ x, y });
        }
    }

    // fill image with tiles
    for (int i = 0; i < nodes.size(); i++)
    {
        int x_left_top = nodes[i].first;
        int y_left_top = nodes[i].second;
        if (i == tiles.size())
            break;
        for (int x = 0; x < min_w; x++)
        {
            for (int y = 0; y < min_h; y++)
            {
                resultImage[y_left_top + y][x_left_top + x] = tiles[i][y][x];
            }
        }
    }

    string output_path = "image.ppm";
    writeImage(resultImage, output_path);
}
