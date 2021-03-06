#include <iomanip>
#include <vector>
#include <iostream>
#include <omp.h>
#include <fstream>
#include <sstream>
#include <numeric>
#include <cmath>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <iterator>
#include <vector>
#include <utility> // std::pair
#include <stdexcept> // std::runtime_error
#include <chrono>
#include <cstdlib>
using std::chrono::system_clock;
using std::chrono::duration;
using namespace std;

//Die veränderten Punkte werden hierbei in eine CSV gespeichert, diese können dann z.B. mittels Matplotlib graphisch dargestellt werden
void write_csv(std::string filename, std::vector<float> vals){
    std::ofstream myFile("Output/" + filename);
    for(int i = 0; i < vals.size(); ++i)
    {
        myFile << vals.at(i) << "\n";
    }   
    myFile.close();
}

//Zum Einlesen der Startpunkte (Anfangsform von Punkten wird hierdurch vorgegeben)
vector<float> read_csv(string filename){
    vector<float> result;
    ifstream myFile;
    myFile.open("../Input/"+filename);
    string line;
    while(myFile.good())
    {
        getline(myFile, line);
        float num_float = ::atof(line.c_str());
        if (num_float>0){
            result.push_back(num_float);
        }
    }
    myFile.close();
    return result;
}


float s_curve(float s_cu_par){
    float ret_val = 0;
    if (s_cu_par < 0.5){
        ret_val = (2 * s_cu_par*s_cu_par);
        return ret_val;
    }
    else{
        ret_val = s_cu_par * 2 - 1;
        ret_val = (-0.5 * (ret_val*(ret_val-2) - 1));
        return ret_val;
    }
}

// Berechnung von Distanz zwischen zwei Punkten.
float dist(float x1,float y1,float x2,float y2){
    float dist_value = sqrt((pow(x1-x2, 2))+(pow(y1-y2, 2)));
    return dist_value;
}

// Diese Funktion kann als Hilfsmittel genutzt werden um mehr Aufschluss über die entstehenden Vektoren zu bekommen.
void print_vector(std::vector<float> const &input, int size){
    for (int i = 0; i < size; i++) {
        std::cout << input.at(i) << ' ';
    }
    std::cout << std::endl;
}

// Bestimmung der Distanz zwischen einem Punkt (x,y) und der Linie, welche durch die Punkte (x1,y1) und (x2,y2) gegeben ist
float dist_point_line(float x, float y, float x1, float y1, float x2, float y2){
    float dist_line_points = dist(x1,y1,x2,y2);
    float u1 = (((x-x1)*(x2-x1))+((y-y1)*(y2-y1)));
    float u = u1/(dist_line_points*dist_line_points);
    float distance;

    //Hierdurch wird der Fall abgefangen, dass sich der Punkt auf der Geraden (Verlängerte Linie) befindet.
    if (u<0.00001 or u >1){
           float ix = dist(x,y,x1,y1);
           float iy = dist(x,y,x2,y2);
           if (ix > iy){
               distance = iy;
           }
           else{
               distance = ix;
           }
    }
    else{
        float ix = x1+u*(x2-x1);
        float iy = y1+u*(y2-y1);
       distance = dist(x,y,ix,iy);
    }

    return distance;
}

//Mittels dieser Funktion wird der zufällig ausgewählte Punkt leicht verändert und es wird dabei geprüft ob er sich einer gegebenen Form angenährt hat.
//Das Annähern muss nicht zwingend gegeben sein, da zum einen eine gewisse Nähe zur Form ausreichend ist und zum anderen bei do_bad = True jegliche Veränderung akzeptiert wird.
//Ziel von letzterem ist es, nicht in lokalen Extrema stecken zu bleiben und somit auch zwischenzeitliche Verschlechterungen zuzulassen.
//Übergabewerte: Zufällig ausgewählter Punkt (x_point/y_point) - temp Wert (Für do_bad relevant), die gewünschte Form (target) und die Größe des Datensatzes (size)
vector<float> perturb(float x_point, float y_point, float temp, string target, int size){

    string circle = "circle";
    string bullseye = "bullseye";
    string dots = "dots";
    string x = "x";
    string h_lines = "h_lines";
    string v_lines = "v_lines";
    string wide_lines = "wide_lines";
    string high_lines = "high_lines";
    string slant_up = "slant_up";
    string slant_down = "slant_down";
    string star = "star";
    if (target == x){
        float shake = 0.1;
        bool per = true;

        float x1 = 20;
        float y1 = 0;
        float x2 = 100;
        float y2 = 100;

        float x3 = 20;
        float y3 = 100;
        float x4 = 100;
        float y4 = 0;

        float x_per = 0;
        float y_per = 0;

        float dis_old_1 = dist_point_line(x_point,y_point,x1,y1,x2,y2);
        float dis_old_2 = dist_point_line(x_point,y_point,x3,y3,x4,y4);

        float dis_old = min({dis_old_1,dis_old_2});

        float dis_new_1 = 0;
        float dis_new_2 = 0;
        float dis_new = 0;

        bool do_bad = false;
        if (((float) rand()/RAND_MAX)<temp){
            do_bad = true;
        }

        while(per==true){
            x_per = x_point + (((float) rand()/RAND_MAX)*2-1)*shake;
            y_per = y_point + (((float) rand()/RAND_MAX)*2-1)*shake;

            dis_new_1 = dist_point_line(x_per,y_per,x1,y1,x2,y2);
            dis_new_2 = dist_point_line(x_per,y_per,x3,y3,x4,y4);
            dis_new = min({dis_new_1,dis_new_2});

            if ((dis_new < dis_old)&&(x_per > 0)&&(y_per > 0)&&(x_per < 100)&&(y_per < 100)){
                break;
            }
            if ((dis_new < 1)&&(x_per > 0)&&(y_per > 0)&&(x_per < 100)&&(y_per < 100)){
                break;
            }
            if ((do_bad == true)&&(x_per > 0)&&(y_per > 0)&&(x_per < 100)&&(y_per < 100)){
                break;
            }
        }
        vector<float> new_values = {x_per,y_per};
        return new_values;
    }
    if (target == slant_up){
        float shake = 0.1;
        bool per = true;

        float x1 = 0;
        float y1 = 0;
        float x2 = 100;
        float y2 = 100;

        float x3 = 0;
        float y3 = 30;
        float x4 = 70;
        float y4 = 100;

        float x5 = 30;
        float y5 = 0;
        float x6 = 100;
        float y6 = 70;

        float x7 = 50;
        float y7 = 0;
        float x8 = 100;
        float y8 = 50;

        float x9 = 0;
        float y9 = 50;
        float x10 = 50;
        float y10 = 100;

        float x_per = 0;
        float y_per = 0;

        float dis_old_1 = dist_point_line(x_point,y_point,x1,y1,x2,y2);
        float dis_old_2 = dist_point_line(x_point,y_point,x3,y3,x4,y4);
        float dis_old_3 = dist_point_line(x_point,y_point,x5,y5,x6,y6);
        float dis_old_4 = dist_point_line(x_point,y_point,x7,y7,x8,y8);
        float dis_old_5 = dist_point_line(x_point,y_point,x9,y9,x10,y10);

        float dis_old = min({dis_old_1,dis_old_2,dis_old_3,dis_old_4,dis_old_5});

        float dis_new_1 = 0;
        float dis_new_2 = 0;
        float dis_new_3 = 0;
        float dis_new_4 = 0;
        float dis_new_5 = 0;
        float dis_new = 0;

        bool do_bad = false;
        if (((float) rand()/RAND_MAX)<temp){
            do_bad = true;
        }

        while(per==true){
            x_per = x_point + (((float) rand()/RAND_MAX)*2-1)*shake;
            y_per = y_point + (((float) rand()/RAND_MAX)*2-1)*shake;

            dis_new_1 = dist_point_line(x_per,y_per,x1,y1,x2,y2);
            dis_new_2 = dist_point_line(x_per,y_per,x3,y3,x4,y4);
            dis_new_3 = dist_point_line(x_per,y_per,x5,y5,x6,y6);
            dis_new_4 = dist_point_line(x_per,y_per,x7,y7,x8,y8);
            dis_new_5 = dist_point_line(x_per,y_per,x9,y9,x10,y10);

            dis_new = min({dis_new_1,dis_new_2,dis_new_3,dis_new_4,dis_new_5});

            if ((dis_new < dis_old)&&(x_per > 0)&&(y_per > 0)&&(x_per < 100)&&(y_per < 100)){
                break;
            }
            if ((dis_new < 1)&&(x_per > 0)&&(y_per > 0)&&(x_per < 100)&&(y_per < 100)){
                break;
            }
            if ((do_bad == true)&&(x_per > 0)&&(y_per > 0)&&(x_per < 100)&&(y_per < 100)){
                break;
            }
        }
        vector<float> new_values = {x_per,y_per};
        return new_values;
    }
    if (target == slant_down){
        float shake = 0.1;
        bool per = true;

        float x1 = 0;
        float y1 = 100;
        float x2 = 100;
        float y2 = 0;

        float x3 = 0;
        float y3 = 70;
        float x4 = 70;
        float y4 = 0;

        float x5 = 30;
        float y5 = 100;
        float x6 = 100;
        float y6 = 30;

        float x7 = 0;
        float y7 = 50;
        float x8 = 50;
        float y8 = 0;

        float x9 = 50;
        float y9 = 100;
        float x10 = 100;
        float y10 = 50;

        float x_per = 0;
        float y_per = 0;

        float dis_old_1 = dist_point_line(x_point,y_point,x1,y1,x2,y2);
        float dis_old_2 = dist_point_line(x_point,y_point,x3,y3,x4,y4);
        float dis_old_3 = dist_point_line(x_point,y_point,x5,y5,x6,y6);
        float dis_old_4 = dist_point_line(x_point,y_point,x7,y7,x8,y8);
        float dis_old_5 = dist_point_line(x_point,y_point,x9,y9,x10,y10);

        float dis_old = min({dis_old_1,dis_old_2,dis_old_3,dis_old_4,dis_old_5});

        float dis_new_1 = 0;
        float dis_new_2 = 0;
        float dis_new_3 = 0;
        float dis_new_4 = 0;
        float dis_new_5 = 0;
        float dis_new = 0;

        bool do_bad = false;
        if (((float) rand()/RAND_MAX)<temp){
            do_bad = true;
        }

        while(per==true){
            x_per = x_point + (((float) rand()/RAND_MAX)*2-1)*shake;
            y_per = y_point + (((float) rand()/RAND_MAX)*2-1)*shake;

            dis_new_1 = dist_point_line(x_per,y_per,x1,y1,x2,y2);
            dis_new_2 = dist_point_line(x_per,y_per,x3,y3,x4,y4);
            dis_new_3 = dist_point_line(x_per,y_per,x5,y5,x6,y6);
            dis_new_4 = dist_point_line(x_per,y_per,x7,y7,x8,y8);
            dis_new_5 = dist_point_line(x_per,y_per,x9,y9,x10,y10);

            dis_new = min({dis_new_1,dis_new_2,dis_new_3,dis_new_4,dis_new_5});

            if ((dis_new < dis_old)&&(x_per > 0)&&(y_per > 0)&&(x_per < 100)&&(y_per < 100)){
                break;
            }
            if ((dis_new < 1)&&(x_per > 0)&&(y_per > 0)&&(x_per < 100)&&(y_per < 100)){
                break;
            }
            if ((do_bad == true)&&(x_per > 0)&&(y_per > 0)&&(x_per < 100)&&(y_per < 100)){
                break;
            }
        }
        vector<float> new_values = {x_per,y_per};
        return new_values;
    }
    if (target == wide_lines){
        float shake = 0.1;
        bool per = true;

        float x1 = 40;
        float y1 = 0;
        float x2 = 40;
        float y2 = 100;

        float x3 = 60;
        float y3 = 0;
        float x4 = 60;
        float y4 = 100;

        float x_per = 0;
        float y_per = 0;

        float dis_old_1 = dist_point_line(x_point,y_point,x1,y1,x2,y2);
        float dis_old_2 = dist_point_line(x_point,y_point,x3,y3,x4,y4);

        float dis_old = min({dis_old_1,dis_old_2});

        float dis_new_1 = 0;
        float dis_new_2 = 0;
        float dis_new = 0;

        bool do_bad = false;
        if (((float) rand()/RAND_MAX)<temp){
            do_bad = true;
        }

        while(per==true){
            x_per = x_point + (((float) rand()/RAND_MAX)*2-1)*shake;
            y_per = y_point + (((float) rand()/RAND_MAX)*2-1)*shake;

            dis_new_1 = dist_point_line(x_per,y_per,x1,y1,x2,y2);
            dis_new_2 = dist_point_line(x_per,y_per,x3,y3,x4,y4);
            dis_new = min({dis_new_1,dis_new_2});

            if ((dis_new < dis_old)&&(x_per > 0)&&(y_per > 0)&&(x_per < 100)&&(y_per < 100)){
                break;
            }
            if ((dis_new < 1)&&(x_per > 0)&&(y_per > 0)&&(x_per < 100)&&(y_per < 100)){
                break;
            }
            if ((do_bad == true)&&(x_per > 0)&&(y_per > 0)&&(x_per < 100)&&(y_per < 100)){
                break;
            }
        }
        vector<float> new_values = {x_per,y_per};
        return new_values;
    }
    if (target == star){
        float shake = 0.1;
        bool per = true;

        float x1 = 28;
        float y1 = 60;
        float x2 = 52;
        float y2 = 60;

        float x3 = 52;
        float y3 = 60;
        float x4 = 60;
        float y4 = 90;

        float x5 = 60;
        float y5 = 90;
        float x6 = 68;
        float y6 = 60;

        float x7 = 68;
        float y7 = 60;
        float x8 = 92;
        float y8 = 60;

        float x9 = 92;
        float y9 = 60;
        float x10 = 72;
        float y10 = 40;

        float x11 = 72;
        float y11 = 40;
        float x12 = 80;
        float y12 = 10;

        float x13 = 80;
        float y13 = 10;
        float x14 = 60;
        float y14 = 30;

        float x15 = 60;
        float y15 = 30;
        float x16 = 40;
        float y16 = 10;

        float x17 = 40;
        float y17 = 10;
        float x18 = 48;
        float y18 = 40;

        float x19 = 48;
        float y19 = 40;
        float x20 = 28;
        float y20 = 60;

        float x_per = 0;
        float y_per = 0;

        float dis_old_1 = dist_point_line(x_point,y_point,x1,y1,x2,y2);
        float dis_old_2 = dist_point_line(x_point,y_point,x3,y3,x4,y4);
        float dis_old_3 = dist_point_line(x_point,y_point,x5,y5,x6,y6);
        float dis_old_4 = dist_point_line(x_point,y_point,x7,y7,x8,y8);
        float dis_old_5 = dist_point_line(x_point,y_point,x9,y9,x10,y10);
        float dis_old_6 = dist_point_line(x_point,y_point,x11,y11,x12,y12);
        float dis_old_7 = dist_point_line(x_point,y_point,x13,y13,x14,y14);
        float dis_old_8 = dist_point_line(x_point,y_point,x15,y15,x16,y16);
        float dis_old_9 = dist_point_line(x_point,y_point,x17,y17,x18,y18);
        float dis_old_10 = dist_point_line(x_point,y_point,x19,y19,x20,y20);

        float dis_old = min({dis_old_1,dis_old_2,dis_old_3,dis_old_4,dis_old_5,dis_old_6,dis_old_7,dis_old_8,dis_old_9,dis_old_10});

        float dis_new_1 = 0;
        float dis_new_2 = 0;
        float dis_new_3 = 0;
        float dis_new_4 = 0;
        float dis_new_5 = 0;
        float dis_new_6 = 0;
        float dis_new_7 = 0;
        float dis_new_8 = 0;
        float dis_new_9 = 0;
        float dis_new_10 = 0;

        float dis_new = 0;

        bool do_bad = false;
        if (((float) rand()/RAND_MAX)<temp){
            do_bad = true;
        }

        while(per==true){
            x_per = x_point + (((float) rand()/RAND_MAX)*2-1)*shake;
            y_per = y_point + (((float) rand()/RAND_MAX)*2-1)*shake;

            dis_new_1 = dist_point_line(x_per,y_per,x1,y1,x2,y2);
            dis_new_2 = dist_point_line(x_per,y_per,x3,y3,x4,y4);
            dis_new_3 = dist_point_line(x_per,y_per,x5,y5,x6,y6);
            dis_new_4 = dist_point_line(x_per,y_per,x7,y7,x8,y8);
            dis_new_5 = dist_point_line(x_per,y_per,x9,y9,x10,y10);
            dis_new_6 = dist_point_line(x_per,y_per,x11,y11,x12,y12);
            dis_new_7 = dist_point_line(x_per,y_per,x13,y13,x14,y14);
            dis_new_8 = dist_point_line(x_per,y_per,x15,y15,x16,y16);
            dis_new_9 = dist_point_line(x_per,y_per,x17,y17,x18,y18);
            dis_new_10 = dist_point_line(x_per,y_per,x19,y19,x20,y20);


            dis_new = min({dis_new_1,dis_new_2,dis_new_3,dis_new_4,dis_new_5,dis_new_6,dis_new_7,dis_new_8,dis_new_9,dis_new_10});

            if ((dis_new < dis_old)&&(x_per > 0)&&(y_per > 0)&&(x_per < 100)&&(y_per < 100)){
                break;
            }
            if ((dis_new < 0.3)&&(x_per > 0)&&(y_per > 0)&&(x_per < 100)&&(y_per < 100)){
                break;
            }
            if ((do_bad == true)&&(x_per > 0)&&(y_per > 0)&&(x_per < 100)&&(y_per < 100)){
                break;
            }
        }
        vector<float> new_values = {x_per,y_per};
        return new_values;
    }
    if (target == high_lines){
        float shake = 0.1;
        bool per = true;

        float x1 = 0;
        float y1 = 25;
        float x2 = 100;
        float y2 = 25;

        float x3 = 0;
        float y3 = 75;
        float x4 = 100;
        float y4 = 75;

        float x_per = 0;
        float y_per = 0;

        float dis_old_1 = dist_point_line(x_point,y_point,x1,y1,x2,y2);
        float dis_old_2 = dist_point_line(x_point,y_point,x3,y3,x4,y4);

        float dis_old = min({dis_old_1,dis_old_2});

        float dis_new_1 = 0;
        float dis_new_2 = 0;
        float dis_new = 0;

        bool do_bad = false;
        if (((float) rand()/RAND_MAX)<temp){
            do_bad = true;
        }

        while(per==true){
            x_per = x_point + (((float) rand()/RAND_MAX)*2-1)*shake;
            y_per = y_point + (((float) rand()/RAND_MAX)*2-1)*shake;

            dis_new_1 = dist_point_line(x_per,y_per,x1,y1,x2,y2);
            dis_new_2 = dist_point_line(x_per,y_per,x3,y3,x4,y4);
            dis_new = min({dis_new_1,dis_new_2});

            if ((dis_new < dis_old)&&(x_per > 0)&&(y_per > 0)&&(x_per < 100)&&(y_per < 100)){
                break;
            }
            if ((dis_new < 1)&&(x_per > 0)&&(y_per > 0)&&(x_per < 100)&&(y_per < 100)){
                break;
            }
            if ((do_bad == true)&&(x_per > 0)&&(y_per > 0)&&(x_per < 100)&&(y_per < 100)){
                break;
            }
        }
        vector<float> new_values = {x_per,y_per};
        return new_values;
    }
    if (target == h_lines){
        float shake = 0.1;
        bool per = true;

        float x1 = 0;
        float x2 = 100;

        float y1 = 10;
        float y2 = 30;
        float y3 = 50;
        float y4 = 70;
        float y5 = 90;

        float x_per = 0;
        float y_per = 0;

        float dis_old_1 = dist_point_line(x_point,y_point,x1,y1,x2,y1);
        float dis_old_2 = dist_point_line(x_point,y_point,x1,y2,x2,y2);
        float dis_old_3 = dist_point_line(x_point,y_point,x1,y3,x2,y3);
        float dis_old_4 = dist_point_line(x_point,y_point,x1,y4,x2,y4);
        float dis_old_5 = dist_point_line(x_point,y_point,x1,y5,x2,y5);

        float dis_old = min({dis_old_1,dis_old_2,dis_old_3,dis_old_4,dis_old_5});

        float dis_new_1 = 0;
        float dis_new_2 = 0;
        float dis_new_3 = 0;
        float dis_new_4 = 0;
        float dis_new_5 = 0;
        float dis_new = 0;

        bool do_bad = false;
        if (((float) rand()/RAND_MAX)<temp){
            do_bad = true;
        }

        while(per==true){
            x_per = x_point + (((float) rand()/RAND_MAX)*2-1)*shake;
            y_per = y_point + (((float) rand()/RAND_MAX)*2-1)*shake;

            dis_new_1 = dist_point_line(x_per,y_per,x1,y1,x2,y1);
            dis_new_2 = dist_point_line(x_per,y_per,x1,y2,x2,y2);
            dis_new_3 = dist_point_line(x_per,y_per,x1,y3,x2,y3);
            dis_new_4 = dist_point_line(x_per,y_per,x1,y4,x2,y4);
            dis_new_5 = dist_point_line(x_per,y_per,x1,y5,x2,y5);

            dis_new = min({dis_new_1,dis_new_2,dis_new_3,dis_new_4,dis_new_5});

            if ((dis_new < dis_old)&&(x_per > 0)&&(y_per > 0)&&(x_per < 100)&&(y_per < 100)){
                break;
            }
            if ((dis_new < 1)&&(x_per > 0)&&(y_per > 0)&&(x_per < 100)&&(y_per < 100)){
                break;
            }
            if ((do_bad == true)&&(x_per > 0)&&(y_per > 0)&&(x_per < 100)&&(y_per < 100)){
                break;
            }
        }
        vector<float> new_values = {x_per,y_per};
        return new_values;
    }
    if (target == v_lines){
        float shake = 0.1;
        bool per = true;

        float y1 = 0;
        float y2 = 100;

        float x1 = 10;
        float x2 = 30;
        float x3 = 50;
        float x4 = 70;
        float x5 = 90;

        float x_per = 0;
        float y_per = 0;

        float dis_old_1 = dist_point_line(x_point,y_point,x1,y1,x1,y2);
        float dis_old_2 = dist_point_line(x_point,y_point,x2,y1,x2,y2);
        float dis_old_3 = dist_point_line(x_point,y_point,x3,y1,x3,y2);
        float dis_old_4 = dist_point_line(x_point,y_point,x4,y1,x4,y2);
        float dis_old_5 = dist_point_line(x_point,y_point,x5,y1,x5,y2);

        float dis_old = min({dis_old_1,dis_old_2,dis_old_3,dis_old_4,dis_old_5});

        float dis_new_1 = 0;
        float dis_new_2 = 0;
        float dis_new_3 = 0;
        float dis_new_4 = 0;
        float dis_new_5 = 0;
        float dis_new = 0;

        bool do_bad = false;
        if (((float) rand()/RAND_MAX)<temp){
            do_bad = true;
        }

        while(per==true){
            x_per = x_point + (((float) rand()/RAND_MAX)*2-1)*shake;
            y_per = y_point + (((float) rand()/RAND_MAX)*2-1)*shake;

            dis_new_1 = dist_point_line(x_per,y_per,x1,y1,x1,y2);
            dis_new_2 = dist_point_line(x_per,y_per,x2,y1,x2,y2);
            dis_new_3 = dist_point_line(x_per,y_per,x3,y1,x3,y2);
            dis_new_4 = dist_point_line(x_per,y_per,x4,y1,x4,y2);
            dis_new_5 = dist_point_line(x_per,y_per,x5,y1,x5,y2);

            dis_new = min({dis_new_1,dis_new_2,dis_new_3,dis_new_4,dis_new_5});

            if ((dis_new < dis_old)&&(x_per > 0)&&(y_per > 0)&&(x_per < 100)&&(y_per < 100)){
                break;
            }
            if ((dis_new < 1)&&(x_per > 0)&&(y_per > 0)&&(x_per < 100)&&(y_per < 100)){
                break;
            }
            if ((do_bad == true)&&(x_per > 0)&&(y_per > 0)&&(x_per < 100)&&(y_per < 100)){
                break;
            }
        }
        vector<float> new_values = {x_per,y_per};
        return new_values;
    }
    if (target == circle){
        float shake = 0.1;
        bool per = true;
        float cx = 50;
        float cy = 50;
        float r = 30;
        float x_per = 0;
        float y_per = 0;

        float dis_old = abs(dist(x_point,y_point,cx,cy)-r);
        float dis_new = 0;

        bool do_bad = false;
        if (((float) rand()/RAND_MAX)<temp){
            do_bad = true;
        }
        while(per==true){
            x_per = x_point + (((float) rand()/RAND_MAX)*2-1)*shake;
            y_per = y_point + (((float) rand()/RAND_MAX)*2-1)*shake;
            dis_new = abs(dist(x_per,y_per,cx,cy)-r);
            if ((dis_new < dis_old)&&(x_per > 0)&&(y_per > 0)&&(x_per < 100)&&(y_per < 100)){
                break;
            }
            if ((dis_new < 2)&&(x_per > 0)&&(y_per > 0)&&(x_per < 100)&&(y_per < 100)){
                break;
            }
            if ((do_bad == true)&&(x_per > 0)&&(y_per > 0)&&(x_per < 100)&&(y_per < 100)){
                break;
            }
        }
        vector<float> new_values = {x_per,y_per};
        return new_values;
    }
    if (target == dots){
        float shake = 0.1;
        bool per = true;

        float px1 = 25;
        float px2 = 50;
        float px3 = 75;
        float py1 = 20;
        float py2 = 50;
        float py3 = 80;

        float x_per = 0;
        float y_per = 0;

        float dis_old_1 = dist(x_point,y_point,px1,py1);
        float dis_old_2 = dist(x_point,y_point,px1,py2);

        float dis_old_3 = dist(x_point,y_point,px1,py3);
        float dis_old_4 = dist(x_point,y_point,px2,py1);
        float dis_old_5 = dist(x_point,y_point,px2,py2);
        float dis_old_6 = dist(x_point,y_point,px2,py3);
        float dis_old_7 = dist(x_point,y_point,px3,py1);
        float dis_old_8 = dist(x_point,y_point,px3,py2);
        float dis_old_9 = dist(x_point,y_point,px3,py3);

        float dis_old = min({dis_old_1,dis_old_2,dis_old_3,dis_old_4,dis_old_5,dis_old_6,dis_old_7,dis_old_8,dis_old_9});

        float dis_new = 0;

        float dis_new_1 = 0;
        float dis_new_2 = 0;
        float dis_new_3 = 0;
        float dis_new_4 = 0;
        float dis_new_5 = 0;
        float dis_new_6 = 0;
        float dis_new_7 = 0;
        float dis_new_8 = 0;
        float dis_new_9 = 0;


        bool do_bad = false;

        if (((float) rand()/RAND_MAX)<temp){
            do_bad = true;
        }

        while(per==true){
            x_per = x_point + (((float) rand()/RAND_MAX)*2-1)*shake;
            y_per = y_point + (((float) rand()/RAND_MAX)*2-1)*shake;

            dis_new_1 = dist(x_per,y_per,px1,py1);
            dis_new_2 = dist(x_per,y_per,px1,py2);
            dis_new_3 = dist(x_per,y_per,px1,py3);
            dis_new_4 = dist(x_per,y_per,px2,py1);
            dis_new_5 = dist(x_per,y_per,px2,py2);
            dis_new_6 = dist(x_per,y_per,px2,py3);
            dis_new_7 = dist(x_per,y_per,px3,py1);
            dis_new_8 = dist(x_per,y_per,px3,py2);
            dis_new_9 = dist(x_per,y_per,px3,py3);

            dis_new = min({dis_new_1,dis_new_2,dis_new_3,dis_new_4,dis_new_5,dis_new_6,dis_new_7,dis_new_8,dis_new_9});


            if ((dis_new < dis_old)&&(x_per > 0)&&(y_per > 0)&&(x_per < 100)&&(y_per < 100)){
                break;
            }
            if ((dis_new < 3)&&(x_per > 0)&&(y_per > 0)&&(x_per < 100)&&(y_per < 100)){
                break;
            }
            if ((do_bad == true)&&(x_per > 0)&&(y_per > 0)&&(x_per < 100)&&(y_per < 100)){
                break;
            }
        }
        vector<float> new_values = {x_per,y_per};
        return new_values;
    }
    if (target == bullseye){
        float shake = 0.1;
        bool per = true;
        float cx = 50;
        float cy = 50;
        float rs_1 = 18;
        float rs_2 = 37;
        float x_per = 0;
        float y_per = 0;

        float dis_old_1 = abs(dist(x_point,y_point,cx,cy)-rs_1);
        float dis_old_2 = abs(dist(x_point,y_point,cx,cy)-rs_2);
        float dis_old = min(dis_old_1,dis_old_2);

        float dis_new = 0;
        float dis_new_1 = 0;
        float dis_new_2 = 0;

        bool do_bad = false;

        if (((float) rand()/RAND_MAX)<temp){
            do_bad = true;
        }

        while(per==true){
            x_per = x_point + (((float) rand()/RAND_MAX)*2-1)*shake;
            y_per = y_point + (((float) rand()/RAND_MAX)*2-1)*shake;

            dis_new_1 = abs(dist(x_per,y_per,cx,cy)-rs_1);
            dis_new_2 = abs(dist(x_per,y_per,cx,cy)-rs_2);
            dis_new = min(dis_new_1,dis_new_2);


            if ((dis_new < dis_old)&&(x_per > 0)&&(y_per > 0)&&(x_per < 100)&&(y_per < 100)){
                break;
            }
            if ((dis_new < 1)&&(x_per > 0)&&(y_per > 0)&&(x_per < 100)&&(y_per < 100)){
                break;
            }
            if ((do_bad == true)&&(x_per > 0)&&(y_per > 0)&&(x_per < 100)&&(y_per < 100)){
                break;
            }
        }
        vector<float> new_values = {x_per,y_per};
        return new_values;
    }
}


//Mittels dieser Funktion werden die statistischen Werte der Punktemenge ermittelt
vector<float> get_values(vector<float> x_value,vector<float> y_value, int size){

    float sum_x = 0;
    float mean_x = 0;
    float sum_y = 0;
    float mean_y = 0;
    float sq_sum_x = 0;
    float stdev_x = 0;
    float sq_sum_y = 0;
    float stdev_y = 0;

#pragma omp for
    for (int i = 0; i < size; ++i){
        sum_x+=x_value[i];
        sum_y+=y_value[i];
        sq_sum_x+=x_value[i]*x_value[i];
        sq_sum_y+=y_value[i]*y_value[i];
    }

    mean_x = sum_x / size;
    mean_y = sum_y /size;

    stdev_x = sqrt(sq_sum_x /size - mean_x * mean_x);
    stdev_y = sqrt(sq_sum_y / size - mean_y * mean_y);
    
    float pear_fac = 0;

#pragma omp simd reduction(+ : pear_fac)
    for (int i = 0; i < size; ++i){
        pear_fac+=(x_value[i]-mean_x)*(y_value[i]-mean_y);
    }
    float pear_coef = pear_fac/(size*stdev_x*stdev_y);

    vector<float> values = {mean_x,mean_y,stdev_x,stdev_y,pear_coef};

    return values;
}

//Hier wird überprüft, ob die statistschen Werte der veränderten Punktemenge mit der Anfangspunktemenge auf zwei Nachkommastellen übereinstimmen.
bool is_error_still_ok(vector<float> x_start, vector<float> y_start, vector<float> per_x, vector<float> per_y, int de, int size){
    vector<float> values_start = get_values(x_start,y_start,size);
    vector<float> values_per = get_values(per_x,per_y,size); 

    float err = 0;

    for (int i = 0; i<5;++i){
        err += abs(floor(values_start[i]*pow(10, de)) - floor(values_per[i]*pow(10, de)));
    }
    bool bool_err = false;

    if (err == 0){
        bool_err = true;
    }

    return bool_err == true;
}

//Diese Funktion dient als Hilfsmittel um die statistischen Werte auszugeben. (Überprüfung von außen)
void print_values(vector<float> x_start, vector<float> y_start, vector<float> per_x, vector<float> per_y, int size){
    vector<float> values_start = get_values(x_start,y_start,size);
    vector<float> values_per = get_values(per_x,per_y,size); 
    std::cout << "x_mean_start:" << ' ' << values_start[0] << std::endl << "x_mean_end:" << ' ' << values_per[0] << std::endl << std::endl;
    std::cout << "y_mean_start:" << ' ' << values_start[1] << std::endl << "y_mean_end:" << ' ' << values_per[1] << std::endl << std::endl;
    std::cout << "x_std_start:" << ' ' << values_start[2] << std::endl << "x_std_end:" << ' ' << values_per[2] << std::endl << std::endl;
    std::cout << "y_std_start:" << ' ' << values_start[3] << std::endl << "y_std_end:" << ' ' << values_per[3] << std::endl << std::endl;
    std::cout << "corr_start:" << ' ' << values_start[4] << std::endl << "corr_end:" << ' ' << values_per[4] << std::endl << std::endl;
}

//Innerhalb dieser Funktion wird die "perturb" Funktion insgesamt "it" mal aufgerufen
//Wurde per perturb ein neues Punktepaar (x,y) gefunden, so wird überprüft, ob die statistischen Eigenschaften gleich sind (auf zwei Nachkommastellen genau)
//Ist dies gegeben, so wird der Punkt verändert
void run_pattern(vector<float> x_start, vector<float> y_start, string target, int it, int de, int size){
    // As Output the new x and y coordinates would make sense, so it can be ploted with Python
    float max_temp = 0.8;
    float min_temp = 0;
    srand ( time(NULL) );

    vector<float> x_check = x_start;
    vector<float> y_check = y_start;

    for (int i = 1; i < (it+1); i++) {
        float s_cu_par = (it-i)/it;
        float s_cu = s_curve(s_cu_par);
        float t = (max_temp-min_temp)*s_cu + min_temp;
        vector<float> x_perturb = x_start;
        vector<float> y_perturb = y_start;

        //Randomizer liefert hier unerkl�rlicherweise immer die selben Werte?!?!
        int ran_point = rand() % size;

        //std::cout << ran_point << ' ';
        
        vector <float> new_values = perturb(x_start[ran_point],y_start[ran_point],t,target,size);

        x_perturb[ran_point] = new_values[0];
        y_perturb[ran_point] = new_values[1];

        bool error_ok = is_error_still_ok(x_start, y_start, x_perturb, y_perturb, de,size);
        if (error_ok == true) {
            x_start[ran_point] = new_values[0];
            y_start[ran_point] = new_values[1];
        }
        //if (error_ok == false) {
        //   std::cout << new_values[0] << ' ';
        //}
    }

    //////////////////////////////////
    // Manual Check of stat. Values //
    //////////////////////////////////

    //print_values(x_check, y_check, x_start, y_start, size);

    ////////////////////
    // Read/Write CSV //
    ////////////////////
    
    string insert_x = "x_" + std::to_string(it) + ".csv";
    string insert_y = "y_" + std::to_string(it) + ".csv";
    write_csv(insert_x, x_start);
    write_csv(insert_y, y_start);

}


int main(int argc, char** argv){
    string shape_start = argv[1];
    string shape_end = argv[2];
    int it_range = atoi(argv[3]);
    int it_step_number = atoi(argv[4]);

    vector<float> x;
    vector<float> y;

    string dino = "dino";
    string angled_blob = "angled_blob";
    string less_angled_blob = "less_angled_blob";
    string random_cloud = "random_cloud";
    string slanted_less = "slanted_less";

    //Je nach gewünschter Startform, werden hierbei die csv Dateien eingelesen. (Startwerte)
    if (shape_start == dino){
        x = read_csv("x_datasaurus.csv");
        y = read_csv("y_datasaurus.csv");
    }

    if (shape_start == angled_blob){
        x = read_csv("x_angled_blob.csv");
        y = read_csv("y_angled_blob.csv");
    }

    if (shape_start == less_angled_blob){
        x = read_csv("x_less_angled_blob.csv");
        y = read_csv("y_less_angled_blob.csv");
    }

    if (shape_start == random_cloud){
        x = read_csv("x_random_cloud.csv");
        y = read_csv("y_random_cloud.csv");
    }

    if (shape_start == slanted_less){
        x = read_csv("x_slanted_less.csv");
        y = read_csv("y_slanted_less.csv");
    }

    int de = 2;
    int size = x.size();

    // Hier werden dann it_step_number viele CSV's erzeugt, welche sich jeweils um it_range Iterationen im Fortschritt des Algorithmus unterscheiden
    for(int i = 1; i < it_step_number; ++i)
    {
        int it = i*it_range;
        run_pattern(x,y,shape_end,it,de,size);
    }   
}

