/*
 *   C++ UDP socket client for live image upstreaming
 *   Modified from http://cs.ecs.baylor.edu/~donahoo/practical/CSockets/practical/UDPEchoClient.cpp
 *   Copyright (C) 2015
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "PracticalSocket.h"      // For UDPSocket and SocketException
#include <iostream>               // For cout and cerr
#include <cstdlib>                // For atoi()
#include <stdio.h>
#include <unistd.h>

using namespace std;

#include "opencv2/opencv.hpp"
using namespace cv;
#include "config.h"

//18:37
#include <pthread.h>
#define NUM_THREADS 2
#define SLEEP_TIME 30000
//VideoCapture cap(1); // Grab the camera
//Mat frame,send;
vector < uchar > encoded2;
vector < uchar > encoded1;
clock_t time_init0=clock();
int time_count=0;
int startFlag=0;
/*
struct thread_data{
    string servAddress_t;
    unsigned short servPort_t;
};
*/
/*
struct thread_count{
    int thread1_count=0;
    int thread2_count=0;
}thread_count;
*/
/*
void* thread(void *threadarg)
{
    struct thread_data *my_data;

    my_data=(struct thread_data *) threadarg;

*/
void* thread1(void* args)
{
    //VideoCapture cap(1); // Grab the camera
    string servAddress1 = "127.0.0.1"; // First arg: server address
    unsigned short servPort1 = Socket::resolveService("10000", "udp");
    try {
        UDPSocket sock1;
        //int jpegqual1 =  ENCODE_QUALITY; // Compression Parameter
        int time_count1=0;
        clock_t last_cycle1 = clock();
        clock_t next_cycle1 = clock();
        //cap >> frame1;
        while (1) {
           if(startFlag==1&&time_count1<time_count)
           {
               time_count1=time_count;
            int total_pack1 = 1 + (encoded1.size() - 1) / PACK_SIZE;
            int ibuf1[1];
            ibuf1[0] = total_pack1;
            sock1.sendTo(ibuf1, sizeof(int), servAddress1, servPort1);

            for (int i = 0; i < total_pack1; i++)
                sock1.sendTo( & encoded1[i * PACK_SIZE], PACK_SIZE, servAddress1, servPort1);
            //usleep(SLEEP_TIME);
            //waitKey(FRAME_INTERVAL);
            next_cycle1 = clock();
            double duration1 = (next_cycle1 - last_cycle1) / (double) CLOCKS_PER_SEC;
            //cout << "\t1effective FPS:" << (1 / duration1) << " \tkbps:" << (PACK_SIZE * total_pack1 / duration1 / 1024 * 8) << endl;
            //cout << next_cycle1 - last_cycle1;
            last_cycle1 = next_cycle1;
           }
           else
           {
                usleep(SLEEP_TIME);
           }
            
        }
        // Destructor closes the socket

    } catch (SocketException & e) {
        cerr << e.what() << endl;
        exit(1);
    }
}

void* thread2(void* args)
{
    //VideoCapture cap(1); // Grab the camera
    string servAddress2 = "127.0.0.1"; // First arg: server address
    unsigned short servPort2 = Socket::resolveService("10002", "udp");
    try {
        UDPSocket sock2;
        //int jpegqual2 =  ENCODE_QUALITY; // Compression Parameter
        int time_count2=0;
        clock_t last_cycle2 = clock();
        clock_t next_cycle2 = clock();
        //cap >> frame2;
        while (1) {
            //cout<<"thread2"<<endl;
            if(startFlag==1&&time_count2<time_count)
            {
                time_count2=time_count;
            int total_pack2= 1 + (encoded2.size() - 1) / PACK_SIZE;
            int ibuf2[1];
            ibuf2[0] = total_pack2;
            sock2.sendTo(ibuf2, sizeof(int), servAddress2, servPort2);

            for (int i = 0; i < total_pack2; i++)
                sock2.sendTo( & encoded2[i * PACK_SIZE], PACK_SIZE, servAddress2, servPort2);

            //waitKey(FRAME_INTERVAL);
            //usleep(SLEEP_TIME);

            next_cycle2 = clock();
            double duration2 = (next_cycle2 - last_cycle2) / (double) CLOCKS_PER_SEC;
            //cout << "\t2effective FPS:" << (1 / duration2) << " \tkbps:" << (PACK_SIZE * total_pack2 / duration2 / 1024 * 8) << endl;
            //cout << next_cycle2 - last_cycle2;
            last_cycle2 = next_cycle2;
            }
            else
            {
                usleep(SLEEP_TIME);
            }
            
        }
        // Destructor closes the socket

    } catch (SocketException & e) {
        cerr << e.what() << endl;
        exit(1);
    }
}

int main(int argc, char * argv[]) {
    if ((argc < 3) || (argc > 3)) { // Test for correct number of arguments
        cerr << "Usage: " << argv[0] << " <Server> <Server Port>\n";
        exit(1);
    }

    string servAddress = argv[1]; // First arg: server address
    unsigned short servPort = Socket::resolveService(argv[2], "udp");

    pthread_t tids[NUM_THREADS];
    //struct thread_data td[NUM_THREADS];
    
   //cout<<sizeof(int)<<endl;
   
   int ret1 = pthread_create(&tids[0],NULL,thread1,NULL);
   if(ret1!=0)
   {
       cout << "pthread_create error:error_code="<<ret1<<endl;
   }
   else
    cout<<"Thread1 create successfully."<<endl;
   int ret2 = pthread_create(&tids[1],NULL,thread2,NULL);
   if(ret2!=0)
   {
       cout << "pthread_create error:error_code="<<ret2<<endl;
   }
   else
    cout<<"Thread2 create successfully."<<endl;
     try {
         cout<<"Go"<<endl;
        //UDPSocket sock;
        int jpegqual =  ENCODE_QUALITY; // Compression Parameter

        Mat frame, send;
        //vector < uchar > encoded;
        VideoCapture cap(0); // Grab the camera
        namedWindow("send", CV_WINDOW_AUTOSIZE);
        if (!cap.isOpened()) {
            cerr << "OpenCV Failed to open camera";
            exit(1);
        }

        //clock_t last_cycle = clock();

        while (1) {
            cap >> frame;
            if(frame.size().width==0)continue;//simple integrity check; skip erroneous data...
            resize(frame, send, Size(FRAME_WIDTH, FRAME_HEIGHT), 0, 0, INTER_LINEAR);
            vector < int > compression_params;
            compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
            compression_params.push_back(jpegqual);

            imencode(".jpg", send, encoded1, compression_params);
            imencode(".jpg", send, encoded2, compression_params);
            imshow("send", send);
            //cout<<"Go"<<endl;
            startFlag=1;
            time_count++;
            waitKey(FRAME_INTERVAL);
            //usleep(SLEEP_TIME);
           startFlag=0;
        }
        // Destructor closes the socket

    } catch (SocketException & e) {
        cerr << e.what() << endl;
        exit(1);
    }
    pthread_exit(NULL);
    return 0;
}
