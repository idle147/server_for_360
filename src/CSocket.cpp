#include "CSocket.h"

//= = = = = = = = = = = = = = = = = = = =
// @函数名称:  CSocketBase(CHostSetting* hostSet)
// @函数说明:  CHostSetting* 类指针, 进行主机的类的设置
// @参数说明:  无
// @返回值:    无
//= = = = = = = = = = = = = = = = = = = =
CSocketBase::CSocketBase()
{
	//初始化套接字标识符
	this->socket_fd = 0;
	this->this_addr = nullptr;
	short ipv4 = (short)strcasecmp(DOC_GET_STR("server_set", "server_protocol_type"), "ipv4");
	short ipv6 = (short)strcasecmp(DOC_GET_STR("server_set", "server_protocol_type"), "ipv6");
	//判断端口的合法性
	if (JudgePort() == false)
	{
		cerr << "输入的端口地址不合法" << endl;
	}
	//判断IP地址的合法性
	else if (ipv4 != 0 && ipv6 != 0)
	{
		cerr << "输入协议类型不合法, 请输入ipv4/ipv6" << endl;
	}
	//如果是IPV4,且合法
	if (ipv4 == 0 && JudgeIpv4() == true)
	{
		//创建设置类型的指针
		SetIpv4();
		return;
	}
	//如果是ipv6,且合法
	else if (ipv6 == 0 && JudgeIpv6() == false)
	{
		//创建设置类型的指针
		SetIpv6();
		return;
	}
	journal->ErrMsgWriteLog("服务器地址设置不合法");
	exit(0);
}

//= = = = = = = = = = = = = = = = = = = =
// @函数名称:  ~CSocketBase()
// @函数说明:  析构函数,用以删除无用的对象
// @参数说明:  无
// @返回值:    无
//= = = = = = = = = = = = = = = = = = = =
CSocketBase::~CSocketBase()
{
	//删除对象
	if (this->this_addr != nullptr)
	{
		delete (struct sockaddr_in*)this->this_addr;
		this->this_addr = nullptr;
	}
}

//= = = = = = = = = = = = = = = = = = = =
// @函数名称:  Start()
// @函数说明:  启动函数,虚函数,默认采用非阻塞式的TCP连接协议
// @参数说明:  无
// @返回值:    无
//= = = = = = = = = = = = = = = = = = = =
void CSocketBase::Start()
{
	//判断端口是否被占用
	if (this->isListening(DOC_GET_STR("server_set", "server_ip_address"),
		std::to_string(DOC_GET_INT("server_set", "server_port"))) == true)
	{
		cerr << "端口被占用,请使用指令强制结束占用后重试" << endl;
		cout << "前置服务器, 启动失败" << endl;
		exit(0);
	}
	//设置套接字的相关参数,默认非阻塞TCP协议
	this->socket_fd = socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
	if (this->socket_fd < 0)
	{
		cerr << "套接字创建出错" << endl;
		return;
	}
	cout << "套接字创建成功" << endl;
	this->Run();
}

//= = = = = = = = = = = = = = = = = = = =
// @函数名称:  Run()
// @函数说明:  纯虚函数, 重写run
// @参数说明:  无
// @返回值:    无
//= = = = = = = = = = = = = = = = = = = =
void CSocketBase::Run()
{
	//纯虚函数, 重写run
}

//= = = = = = = = = = = = = = = = = = = =
// @函数名称:  Stop()
// @函数说明:  纯虚函数, 重写stop
// @参数说明:  无
// @返回值:    无
//= = = = = = = = = = = = = = = = = = = =
void CSocketBase::Stop()
{
	//纯虚函数, 重写stop
}

//= = = = = = = = = = = = = = = = = = = =
// @函数名称:  JudgeIpv4()
// @函数说明:  判断ipv4地址的合法性
// @参数说明:  无
// @返回值:    无
//= = = = = = = = = = = = = = = = = = = =
bool CSocketBase::JudgeIpv4()
{
	const char* ipv4 = DOC_GET_STR("server_set", "server_ip_address");
	if (!ipv4 || !ipv4[0]) {
		return false;
	}

	size_t len = strlen(ipv4);
	if (len < 7 || len > 15) {
		return false;
	}

	int num[4] = { 0 };
	char c = 0;

	if (sscanf(ipv4, "%d.%d.%d.%d%c", num, num + 1, num + 2, num + 3, &c) != 4)
	{
		return false;
	}

	int i = 0;
	for (; i < 4; i++)
	{
		if (num[i] < 0 || num[i] > 255)
		{
			return false;
		}
	}
	return true;
}

//= = = = = = = = = = = = = = = = = = = =
// @函数名称:  JudgeIpv6()
// @函数说明:  判断ipv6地址的合法性
// @参数说明:  无
// @返回值:    无
//= = = = = = = = = = = = = = = = = = = =
bool CSocketBase::JudgeIpv6()
{
	//待实现
	cerr << "ipv6还未实现" << endl;
	return false;
}

std::string CSocketBase::isPortOpen(const std::string& domain, const std::string& port)
{
	addrinfo* result;
	addrinfo hints{};
	hints.ai_family = AF_UNSPEC;   // either IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM;
	char addressString[INET6_ADDRSTRLEN];
	const char* retval = nullptr;
	if (0 != getaddrinfo(domain.c_str(), port.c_str(), &hints, &result)) {
		return "";
	}
	for (addrinfo* addr = result; addr != nullptr; addr = addr->ai_next) {
		int handle = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
		if (handle == -1) {
			continue;
		}
		if (connect(handle, addr->ai_addr, addr->ai_addrlen) != -1) {
			switch (addr->ai_family) {
			case AF_INET:
				retval = inet_ntop(addr->ai_family, &(reinterpret_cast<sockaddr_in*>(addr->ai_addr)->sin_addr), addressString, INET6_ADDRSTRLEN);
				break;
			case AF_INET6:
				retval = inet_ntop(addr->ai_family, &(reinterpret_cast<sockaddr_in6*>(addr->ai_addr)->sin6_addr), addressString, INET6_ADDRSTRLEN);
				break;
			default:
				// unknown family
				retval = nullptr;
			}
			close(handle);
			break;
		}
	}
	freeaddrinfo(result);
	return retval == nullptr ? "" : domain + ":" + retval + "\n";
}

bool CSocketBase::isListening(const std::string& ip, const std::string& port)
{
	std::string addr = isPortOpen(ip, port);
	if (addr.empty()) {
		return false;
	}
	return true;
}

bool CSocketBase::killPort()
{
	int port = DOC_GET_INT("server_set", "server_port");
	string str = "pid=`lsof -i:";
	str += std::to_string(port);
	str += "| awk 'NR>=2{print $2}'`; kill -9 $pid;";
	system(str.c_str());
}

//= = = = = = = = = = = = = = = = = = = =
// @函数名称:  JudgePort()
// @函数说明:  判断端口地址的合法性
// @参数说明:  无
// @返回值:    无
//= = = = = = = = = = = = = = = = = = = =
bool CSocketBase::JudgePort()
{
	int port = DOC_GET_INT("server_set", "server_port");
	if (port < 1024 || port > 65535)
	{
		return false;
	}
	return true;
}

//= = = = = = = = = = = = = = = = = = = =
// @函数名称:  SetIpv4()
// @函数说明:  设置ipv4地址
// @参数说明:  无
// @返回值:    无
//= = = = = = = = = = = = = = = = = = = =
void CSocketBase::SetIpv4()
{
	//如果已经存在地址信息,则删除
	if (this_addr != nullptr)
	{
		delete (struct sockaddr_in*)this->this_addr;
		this_addr = nullptr;
	}
	//初始化ipv4结构体,并设置信息
	struct sockaddr_in* ipv4Addr = new struct sockaddr_in; //新建ipv4地址信息
	bzero(ipv4Addr, sizeof(struct sockaddr_in));           //初始化ipv4地址信息
	ipv4Addr->sin_family = AF_INET;                        //设置协议族为IPV4类型
	ipv4Addr->sin_addr.s_addr = inet_addr(DOC_GET_STR("server_set", "server_ip_address"));//设置IP地址
	ipv4Addr->sin_port = htons(DOC_GET_INT("server_set", "server_port")); //创建服务器指定端口号
	this->this_addr = ipv4Addr;
}

void CSocketBase::SetIpv6()
{
	if (this_addr != nullptr)
	{
		delete (struct sockaddr_in*)(this->this_addr);
		this_addr = nullptr;
	}
	//初始化ipv6结构体,并设置信息
	struct sockaddr_in6* ipv6Addr = new struct sockaddr_in6;    //ipv6地址信息;
	cerr << "ipv6待完善" << endl;
	this->this_addr = ipv6Addr;
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////服务器类相关////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//= = = = = = = = = = = = = = = = = = = =
// @函数名称:  CServerSocket()
// @函数说明:  调用父类构造函数后,再调用子类构造函数
// @参数说明:  主机设置
// @返回值:    无
//= = = = = = = = = = = = = = = = = = = =
CServerSocket::CServerSocket() :CSocketBase()
{
	this->maxNum = DOC_GET_INT("server_set", "max_connectNum");
	this->Start();
}

//= = = = = = = = = = = = = = = = = = = =
// @函数名称:  Run()
// @函数说明:  重写Run函数
// @参数说明:  无
// @返回值:    无
//= = = = = = = = = = = = = = = = = = = =
void CServerSocket::Run()
{
	//获取套接字
	struct sockaddr_in tempAddr = *(struct sockaddr_in*)this->this_addr;
	//设置接受任意IP地址的客户连接
	tempAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//端口重用,允许立即使用端口地址,无需等待TIME_WAIT
	int res = 1;
	if (setsockopt(this->socket_fd, SOL_SOCKET, SO_REUSEADDR, &res, sizeof(res)) < 0)
	{
		cerr << "套接字重用错误" << endl;
		return;
	}
	//将套接字进行绑定
	res = bind(this->socket_fd, (struct sockaddr*)&tempAddr, sizeof(struct sockaddr_in));
	if (res == -1)
	{
		journal->ErrMsgWriteLog("绑定套接字错误:");
		return;
	}
	journal->WriteLog("前置服务器, 绑定套接字成功!");
	//进行监听
	res = listen(socket_fd, this->maxNum);
	if (res == -1)
	{
		journal->ErrMsgWriteLog("监听发生错误:");
		return;
	}
	journal->WriteLog("服务器监听中...");
}

//= = = = = = = = = = = = = = = = = = = =
// @函数名称:  Stop()
// @函数说明:  关闭套接字
// @参数说明:  无
// @返回值:    无
//= = = = = = = = = = = = = = = = = = = =
void CServerSocket::Stop()
{
	if (this->socket_fd != 0)
	{
		close(socket_fd);
	}
}