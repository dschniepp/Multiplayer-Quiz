//all_socket.c
#include  "all_socket.h"

/*--------------Client Functions---------------------*/
 
/*Function to connect sockets
Return 1 = Succesful
Return 0 = Error
*/
 
int connect_socket(int *sock, char *serv_addr, unsigned short port)
 
	struct addrinfo *addr_info, *p, hints;
	int ret;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = 0 /* | AI_ADDRCONFIG */;
	
	
	/* RTFM: getaddrinfo */
        ret = getaddrinfo(serv_addr, port, &hints, &addr_info);
        if (ret)
	{
		errorPrint("getaddrinfo: %s", gai_strerror(ret));
		exit(1);
	}

        p = addr_info;
        while (p)
        {
		int s;
		char dst[INET6_ADDRSTRLEN];

		/* Create socket for found family */		
                s = socket(p->ai_family, p->ai_socktype, 0);

		/* RTFM: getnameinfo */
		getnameinfo(p->ai_addr,
			p->ai_addrlen,
			dst,
			sizeof(dst),
			NULL,
			0,
			NI_NUMERICHOST);

		/* Try to connect */
                if (connect(s, p->ai_addr, p->ai_addrlen) == 0)
                {
                        //infoPrint("Connected");
						return 1;
				}else{
						//errorPrint("Error, while trying %s",dst);
						return 0;
				}

