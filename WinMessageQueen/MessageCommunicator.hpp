#include <windows.h>
#include <string>
#include <functional>
#include <stdexcept>

class MessageCommunicator {
public:
    MessageCommunicator(const std::wstring& windowClassName, std::function<void(const std::string&)> onMessageReceived)
        : onMessageReceived(onMessageReceived) {
        // Register window class
        WNDCLASS wc = {};
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = windowClassName.c_str();
        wc.cbWndExtra = sizeof(MessageCommunicator*);

        if (!RegisterClass(&wc)) {
            throw std::runtime_error("Failed to register window class");
        }

        // Create window
        hwnd = CreateWindowEx(
            0,
            windowClassName.c_str(),
            L"Message Communicator Window",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            NULL,
            NULL,
            GetModuleHandle(NULL),
            this
        );

        if (!hwnd) {
            throw std::runtime_error("Failed to create window");
        }

        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);
    }

    void sendMessage(const std::wstring& targetWindowClassName, const std::string& message) {
        HWND hwndReceiver = FindWindow(targetWindowClassName.c_str(), NULL);
        if (!hwndReceiver) {
            throw std::runtime_error("Receiver window not found");
        }

        COPYDATASTRUCT cds;
        cds.dwData = 1;
        cds.cbData = message.size() + 1;
        cds.lpData = (void*)message.c_str();

        SendMessage(hwndReceiver, WM_COPYDATA, (WPARAM)hwnd, (LPARAM)&cds);
    }

    void runMessageLoop() {
        MSG msg = {};
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    void exitMessageLoop() {
        PostQuitMessage(0);
    }

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        if (uMsg == WM_CREATE) {
            CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
            MessageCommunicator* pThis = (MessageCommunicator*)pCreate->lpCreateParams;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
        }
        else {
            MessageCommunicator* pThis = (MessageCommunicator*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
            if (pThis) {
                return pThis->handleMessage(uMsg, wParam, lParam);
            }
        }
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
        if (uMsg == WM_COPYDATA) {
            PCOPYDATASTRUCT pcds = (PCOPYDATASTRUCT)lParam;
            if (pcds->dwData == 1) {
                std::string receivedMessage((char*)pcds->lpData, pcds->cbData);
                onMessageReceived(receivedMessage);
            }
            return 0;
        }
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    HWND hwnd;
    std::function<void(const std::string&)> onMessageReceived;
};
