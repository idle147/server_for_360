#ifndef PACKET_H
#define PACKET_H
/*
//////////////////////////////////////////////////////
不完全的总结
	1.客户端要发送的包头类型包:
		登陆包、注册包、视频包、请求获取视频帧数包、文件片有包、文件片无包

	2.客户端要接收的包头类型有：
		包头类型（packet_type）：响应包
			包体数值（anser_type）：（登陆响应包、注册响应包、视频响应包、文件响应包）
			包体数值（anser_type）：（登陆重发包、注册重发包、视频重发包、文件重发包）
		包头类型（packet_type）：视频帧数包
			包体数值：视频id，用户id，视频帧数
	3.发送的结构体都要有包头, 接收的结构体不要有包头,请自行修改
	4.CRC校验码，函数输入可以直接拿 接收包体的结构体，包头.包大小 生成

//////////////////////////////////////////////////////
文件业务说明:
前置添加说明:
	1.添加结构体
		//文件包
		typedef struct t_PackHead_firstfilepack
		{
			t_PackHead head;       //包头
			char file_name[16];    //文件名
			int	user_id;           //用户主键ID
			int fileSize;          //文件尺寸
			char md5[32];          //MD5校验码
		}t_FirstFilePack;

	2.添加宏定义
		#define FILE_OK 10243
		#define FILE_START 10244
		#define DONT_START 10245
		#define MD5_ERROR 10246
		#define ReqFile  1025    //包头类型

客户端：
	1.生成图片文件
	2.根据Packet.h,规定的文件大小,进行分割
	3.往服务器发送t_FirstFilePack,等待接收响应包
		数据为FILE_START, 则进行下一步
		数据为DONT_START, 则弹出提示,服务器目前无法上传

	4.开始往客户端发包t_FilePack,首包的起始标记包标号记为0
	5.循环发包, 发完包后等待接收包,
	6.接收到请求文件重发包, 根据包号发送相对应的临时文件
	7.接收到为响应包类型为FILE_OK的包, 则弹出上传成功
	  接收到MD5_ERROR, 则弹出文件上传校验失败提示
	8.删除临时文件,结束线程任务
	9.定时内未收到包，结束线程，不删除临时文件
	注：如果在线程外收到文件响应包， 向服务器发送t_FilePack结构体包,
			int类型数据记为user_id, char数组记为文件名, 包头为ReqFile
		服务器会向客户端发t_VideoInfo结构体包,
			int user_id;           //用户ID
			char video_id[16];     //文件名称
			int current_frame;     //请求的包号
			包头为ReqFile
		根据信息调取对应的临时文件
//////////////////////////////////////////////////////

*/
//基本功能包头类型信息
#define LOGIN 101                     //登陆
#define REGISTER 102                  //注册
#define GET_PREGRORESS 103            //获取当前帧数包
#define REQUIRE_PREPGRORESS 104       //请求获取帧数包
#define INSERT_VIDEO_INFO    105      //插入视频信息包
#define DELETE_VIDEO_INFO 106         //删除视频
#define FILE_FIRST_PACK 107           //定义文件的第一个包
#define FILE_SEG_EXIST 108            //定义文件分片的包,且包后还有包--记为片有
#define FILE_SEG_NULL  109            //定义文件分片的包,且包后没有包--记为片无

//基本功能包头类型信息
#define LOGIN 101                     //登陆
#define REGISTER 102                  //注册
#define GET_PREGRORESS 103            //获取当前帧数包
#define REQUIRE_PREPGRORESS 104       //请求获取帧数包
#define INSERT_VIDEO_INFO    105      //插入视频信息包
#define DELETE_VIDEO_INFO 106         //删除视频
#define FILE_FIRST_PACK 107           //定义文件的第一个包
#define FILE_SEG_EXIST 108            //定义文件分片的包,且包后还有包--记为片有
#define FILE_SEG_NULL  109            //定义文件分片的包,且包后没有包--记为片无

//应答包包头信息
#define ANSWER 1003                              //响应包--包头信息
#define LOGIN_ANSWER 100301              //登陆响应包
#define REG_ANSWER 100302                   //注册响应包
#define SELECT_VIDEO_ANSWER 100303  //查询视频响应包
#define DELETE_VIDEO_ANSWER 100304  //删除视频回应包
#define INSERT_VIDEO_ANSWER 100305  //插入视频响应包
#define FILE_ANSWER 100306          //文件回答
//应答包包体信息（一）
#define ANSWER_RESEND 10031            //响应重发包
#define ANSWER_LOGIN_SUCCESS 10032     //应答包回复登录成功
#define ANSWER_REG_SUCCESS 10033       //应答包回复注册成功
#define ANSWER_INSERT_SUCCESS 10034    //应答包回复插入视频信息成功
#define ANSWER_DELETE_SUCCESS 10035    //应答包回复删除视频
#define ANSWER_FILE_START 10036        //应答包回复开始上传文件
#define ANSWER_FILE_SUCCESS 10037      //应答包回复上传文件成功
//应答包包体信息（二）
#define ANSWER_NAME_ERROR -10031      //应答包回复名字错误
#define ANSWER_KEY_ERROR -10032       //应答包回复密码错误
#define ANSWER_DELETE_ERROR -10033    //应答包删除视频错误
#define ANSWER_QUERY_ERROR -10034     //应答包回复查无视频信息
#define ANSWER_INSERT_ERROR -10035    //应答包回复插入视频信息错误
#define ANSWER_FILE_START_ERROR -10036//应答包因为服务发生错误,[别上传]
#define ANSWER_FILE_ERROR -10037         //应答包因为服务发生错误,上传失败
#define ANSWER_MD5_ERROR -10038       //应答包回复上传文件MD5错误

//重发包包头类型信息--CRC校验码出错
#define CRC_ERR_RESEND 1004                 //重发包--包头类型
//重发包包体类型信息
#define RESEND_LOGIN 10041                  //登陆重发包
#define RESEND_REGISTER 10042               //注册重发包
#define RESEND_GET_PREGRORESS 10043         //获取当前帧数包重发包
#define RESEND_REQUIRE_PREPGRORESS 10044    //请求获取帧数包重发包
#define RESEND_INSERT    10045              //插入视频信息包重发包
#define RESEND_DELETE 10046                 //删除视频重发包
#define RESEND_FILE_FIRST_PACK 10047        //定义文件的第一个包
#define RESEND_FILE_SEG_EXIST 10048         //定义文件分片的包,且包后还有包--记为片有
#define RESEND_FILE_SEG_NULL 100419         //请求重发片有文件包(CRC校验出错)

//文件重发包包头类型信息
#define FILE_RESEND 1005              //请求重发文件包(没有收到指定文件)

//相关参数宏定义
#define MAX_CHAR_LEN 256               //char数组最长大小设置
#define PACK_MAX_LEN 1500              //数据包包体的最长长度
#define FILE_LEN 1024                  //定义传输的文件的最长长度
#define SQL_LEN 255                    //定义数据库数组的长度字段
#define SQLOPT_SUCCESS 1233            //数据库操作成功
#define SQLOPT_ERROR -1233             //定义sql语句操作出错
#define RollBack 852                   //进行回退操作
#define MAX_PATH_LEN 512              //最大路径长度
/*
	发送文件思路:
		1. 如果是最后一片, 包头类型设为片无, 否则设为片有
		2. 包头大小设为文件具体的大小,最后一个包不一定是1024长度
		3. 流水号为每一片的编号
		4. CRC校验码设置
		5. 包体里面写入数据
*/

//包头
typedef struct t_PackHead_head
{
	int packet_type;       //包类型
	int packet_size;       //包大小--包含包头+包体
	int packet_CRC;        //CRC校验码
	int packet_num;        //定义包的编号
}t_PackHead;

//暂定的方法:
//1.接收包的结构体, 接收包后，利用字符串数组存储(recv)
//2.然后强转成包头结构体(memcpy)
//3.接着利用包头的结构体的类型来判断收到什么包(switch)
//4.然后将整个字符串数组内存拷贝成对应的包(memcpy)

//登录包 1003
typedef struct t_PackHead_log
{
	int user_id;                   //用户账号
	char pwd[16];                  //密码
}t_Login;

//注册包
typedef struct t_PackHead_reg
{
	int user_id;            //用户账号
	char name[16];          //用户名
	char pwd[16];           //用户密码
}t_Register;

//应答包
typedef struct t_PackHead_answer
{
	t_PackHead head;        //包头
	int anser_type;         //应答包的类型
}t_Answer;

/*
//登陆后,客户端遍历数据库的视频, 分页等(利用二阶段的代码, 可以全部白嫖)。
点击视频的时候，判断能不能打开，
	能打开，向服务器申请获取视频当前帧数判断与本地帧数是否相同
		相同，播放视频
		不相同，弹出窗口，让客户判断是以本地还是以服务器为准
	不能打开，弹出窗口告知用户，向服务器发送删除该段视频信息包，本地删除该信息
*/

//请求获取视频当前帧数---请求删除当前视频信息(根据包头的类型而定)
typedef struct t_PackHead_requireprogress
{
	int user_id;            //用户编号
	char video_id[20];      //视频编号
}t_RequireProgress;

//插入视频信息包
typedef struct t_PackHead_videoInfo
{
	int user_id;           //用户ID
	int current_frame;     //视频当前帧数
	char video_id[20];     //视频ID
}t_VideoInfo;

//获取视频当前帧数
typedef struct t_PackHead_getprogress
{
	t_PackHead head;        //包头
	int video_progress;     //视频当前帧数
	int user_id;            //用户编号
	char video_id[20];      //视频编号
}t_GetProgress;

//文件包
typedef struct t_PackHead_filepack
{
	t_PackHead head;       //包头
	int packer_num;        //当前包的流水号
	int	user_id;           //用户主键ID
	char file_name[20];    //文件名
	char data[FILE_LEN];   //文件大小
}t_FilePack;

//文件包
typedef struct t_PackHead_firstfilepack
{
	int	user_id;           //用户主键ID
	int fileSize;          //文件大小
	char file_name[20];    //文件名
	char md5[128];          //MD5校验码
}t_FirstFilePack;

#endif