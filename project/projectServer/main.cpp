#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctime>
#include <string>
#include <iostream>

using namespace std;

#define SERVERPORT 9000
#define BUFSIZE    512

//문제 출제하는 클래스 정의
class mathexam{

public:

    //동적할당을 위한 변수 선언
	int * num1;
	int * num2;
	int size;
	char * c;
	int * answer;
	bool * check;


	//생성자 호출시에 받은 값의 크기만큼 동적할당
	mathexam(int size){

		this->size = size;
		num1 = new int[size];
		num2 = new int[size];
		c = new char[size];
		answer = new int[size];
		check = new bool[size];
		setting();

	}

	//소멸자로 동적할당 해제
	~mathexam(){
		delete num1;
		delete num2;
		delete c;
		delete answer;
		delete check;
	}

	//크기만큼 문제를 담는 함수
	void setting(){

		srand((unsigned int)time(NULL));

		for (int i = 0; i < size; i++){
			num1[i] = (rand() % 99) + 1;
			num2[i] = (rand() % 99) + 1;

			int change = (rand() % 3) + 1;

			switch (change){
			case 1:
				c[i] = '+';
				break;
			case 2:
				c[i] = '-';
				break;
			case 3:
				c[i] = '*';
				break;
			case 4:
				c[i] = '/';
				break;
			}
		}
	}

	//답을 변수에 담는 함수
	void setanswer(int i, int a){
	    answer[i] = a;
	}

	// 담은 답과 직접 계산한 답이 맞는지 확인하는 함수
	void checkanswer(){
		int s;
		for (int i = 0; i < size; i++){
			switch (c[i]){
			case '+':
				s = num1[i] + num2[i];
				break;
			case '-':
				s = num1[i] - num2[i];
				break;
			case '*':
				s = num1[i] * num2[i];
				break;
			case '/':
				s = num1[i] / num2[i];
				break;
			}

			if (answer[i] == s){
				check[i] = true;
			}
			else {
				check[i] = false;
			}
		}
	}
};
// 소켓 함수 오류 출력 후 종료
void err_quit(char *msg)
{
      LPVOID lpMsgBuf;
      FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
            NULL, WSAGetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&lpMsgBuf, 0, NULL);
      MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
      LocalFree(lpMsgBuf);
      exit(1);
}

// 소켓 함수 오류 출력
void err_display(char *msg)
{
      LPVOID lpMsgBuf;
      FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
            NULL, WSAGetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&lpMsgBuf, 0, NULL);
      printf("[%s] %s", msg, (char *)lpMsgBuf);
      LocalFree(lpMsgBuf);
}

//quit 를 받으면 강제종료
void checkout(SOCKET s, char * c){

    char *ptr;

    ptr = strtok(c, " ");
    while(ptr != NULL)
    {
        if(!strcmp(ptr,"quit"))
        {
        printf("quit를 받아서 강제 종료 합니다.\n");
        LINGER ling;
        ling.l_onoff = 1;
        ling.l_linger = 0;
        setsockopt(s,SOL_SOCKET,SO_LINGER,(char*)&ling,sizeof(ling));
        closesocket(s);
        break;
        }
        else
        {
            ptr = strtok(NULL, " ");
        }

    }

}

int main(int argc, char *argv[])
{
      int retval;

      // 윈속 초기화
      WSADATA wsa;
      if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
            return 1;

      // socket()
      SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
      if(listen_sock == INVALID_SOCKET)
            err_quit("socket()");

      // bind()
      SOCKADDR_IN serveraddr;
      ZeroMemory(&serveraddr, sizeof(serveraddr));
      serveraddr.sin_family = AF_INET;
      serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
      serveraddr.sin_port = htons(SERVERPORT);
      retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
      if(retval == SOCKET_ERROR)
            err_quit("bind()");

      // listen()
      retval = listen(listen_sock, SOMAXCONN);
      if(retval == SOCKET_ERROR)
            err_quit("listen()");

      // 데이터 통신에 사용할 변수
      SOCKET client_sock;
      SOCKADDR_IN clientaddr;
      int addrlen;
      char buf[BUFSIZE+1];

      while(1)
      {
            // accept()
            addrlen = sizeof(clientaddr);
            client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
            if(client_sock == INVALID_SOCKET)
            {
                  err_display("accept()");
                  break;
            }

            // 접속한 클라이언트 정보 출력
            printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
                   inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));


            //정수형을 통신하기 위한 data
            char data[4];
            int examsize = 0;
            // 문제를 몇개나 제출할 것인가에 대한 수를 받음
            retval = recv(client_sock, data, sizeof(data), 0);
            if(retval == SOCKET_ERROR)
            {
                  err_display("recv()");
                  break;
            }
            else if(retval == 0)
                  break;

            examsize = *((int*)data);

            // 생성자 실행
            mathexam exam = mathexam(examsize);

            //문제를 문자열로 합쳐서 클라이언트에 전송
            for(int i = 0; i < examsize; i++){

                char * n1 = new char[4];
                itoa(exam.num1[i],n1,10);
                char * n2 = new char[4];
                itoa(exam.num2[i],n2,10);
                char * oper = new char[2];
                sprintf(oper,"%c",exam.c[i]);

                strcat(buf, n1);
                strcat(buf, " ");
                strcat(buf, oper);
                strcat(buf, " ");
                strcat(buf, n2);

                retval = send(client_sock, buf, sizeof(buf), 0);

                //각 답을 받아서 저징
                retval = recv(client_sock, data, sizeof(data), 0);
                if(retval == SOCKET_ERROR)
                {
                    err_display("recv()");
                    break;
                }
                else if(retval == 0)
                    break;

                exam.setanswer(i,*((int*)data));

                printf("%d\n",exam.answer[i]);

                buf[0] = '\0';
            }

            //결과 체크 후 클라이언트에 전송
            exam.checkanswer();

            retval = send(client_sock, (char *)exam.check, examsize, 0);

            // closesocket()
            closesocket(client_sock);
            printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
                   inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
      }

      // closesocket()
      closesocket(listen_sock);

      // 윈속 종료
      WSACleanup();
      return 0;
}
