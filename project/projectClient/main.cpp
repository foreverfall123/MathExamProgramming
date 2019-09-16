#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE    512

// ���� �Լ� ���� ��� �� ����
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

// ���� �Լ� ���� ���
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

// ����� ���� ������ ���� �Լ�
int recvn(SOCKET s, char *buf, int len, int flags)
{
	int received;
	char *ptr = buf;
	int left = len;

	while(left > 0){
		received = recv(s, ptr, left, flags);
		if(received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if(received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}

int main(int argc, char *argv[])
{
	int retval;
	int examsize;
	int client_answer;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if(retval == SOCKET_ERROR) err_quit("connect()");

	// ������ ��ſ� ����� ����
	char buf[BUFSIZE+1];
	int len;

	//���ϴ� ���� ������ �Է�
	printf("������ ������ �Է��Ͻÿ� : ");
	scanf("%d", &examsize);

	char data[4] = {0,};
	*((int*)data) = examsize;

    retval = send(sock, data, sizeof(data), 0);
		if(retval == SOCKET_ERROR){
			err_display("send()");
		}

	// ������ ������ ���
	for(int i = 0; i < examsize; i++){

	    retval = recvn(sock, buf, sizeof(buf), 0);
		if(retval == SOCKET_ERROR){
			err_display("recv()");
			break;
		}
		else if(retval == 0)
			break;

			// ���� ������ ���
		buf[retval] = '\0';
		printf("\n%d��° ���� : %s\n",i+1, buf);

		// ���� �ϳ��� �� ���� ����
		printf("�� : ");
		scanf("%d",&client_answer);
		*((int*)data) = client_answer;

		// ������ ������
		retval = send(sock, data, sizeof(data), 0);
		if(retval == SOCKET_ERROR){
			err_display("send()");
			break;
		}
	}

	// �������� �������� üũ�� �迭�� �޾�
	retval = recvn(sock, buf, retval, 0);
		if(retval == SOCKET_ERROR){
			err_display("recv()");
		}

    //���� �� ���
    bool * acheck = new bool[examsize];
    acheck = (bool *)buf;

    for (int i = 0; i < examsize ; i++){
        printf("%d�� ���� ��� : %s\n",i+1,acheck[i]? "����":"����");
    }



	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();
	return 0;
}
