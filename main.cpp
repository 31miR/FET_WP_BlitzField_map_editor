#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include <Windowsx.h>
#include <vector>
#include <iostream>
#include <fstream>

#include "resource.h"

#define DEFAULT_NODE_RADIUS 20
#define MIN_ZOOM 0.25
#define MAX_ZOOM 8

bool points_equal(const POINT&, const POINT&);

struct Path_Connection {
    POINT* pt1;
    POINT* pt2;
    Path_Connection(POINT* pt1, POINT* pt2) : pt1{pt1}, pt2{pt2} {}
    bool operator==(const Path_Connection& other) {
        if (points_equal(*pt1, *other.pt1) && points_equal(*pt2, *other.pt2)) return true;
        if (points_equal(*pt2, *other.pt1) && points_equal(*pt1, *other.pt2)) return true;
        return false;
    }
    bool operator!=(const Path_Connection& other) {
        return !(*this == other);
    }
    bool is_point_in_path(POINT* p) {
        if (points_equal(*pt1, *p)) return true;
        if (points_equal(*pt2, *p)) return true;
        return false;
    }
};

std::string open_file_omg(HWND hwnd, const char* extensions) {
    OPENFILENAME ofn;       // common dialog box structure
    char szFile[260];       // buffer for file name
    HANDLE hf;              // file handle

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    // Set lpstrFile[0] to '\0' so that GetOpenFileName does not
    // use the contents of szFile to initialize itself.
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = extensions;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // Display the Open dialog box.

    if (GetOpenFileName(&ofn)==FALSE)
        return "";
    return std::string{szFile};
}

std::string save_file_omg(HWND hwnd, const char* extensions) {
    OPENFILENAME ofn;       // common dialog box structure
    char szFile[260];       // buffer for file name
    HANDLE hf;              // file handle

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    // Set lpstrFile[0] to '\0' so that GetOpenFileName does not
    // use the contents of szFile to initialize itself.
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = extensions;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // Display the Open dialog box.

    if (GetSaveFileName(&ofn)==FALSE)
        return "";
    return std::string{szFile};
}

void read_and_parse_map(const std::string& path, std::vector<RECT>& high_rects, std::vector<RECT>& low_rects,
                        std::vector<POINT>& path_nodes, std::vector<Path_Connection>& path_connections) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file. Error code: " << strerror(errno) << std::endl;
        return;
    }
    high_rects.clear();
    low_rects.clear();
    path_nodes.clear();
    path_connections.clear();
    char dispose_char; //dummy variable
    /*get all high rects*/
    while (!file.eof()) {
        while (file.peek() == ' ' || file.peek() == '\n') file.ignore();
        if (file.peek() == ';') {
            file >> dispose_char;
            break;
        }
        int a,b,c,d;
        file >> a >> b >> c >> d;
        high_rects.push_back({a,b,c,d});
    }
    /*get all low rects*/
    while (!file.eof()) {
        while (file.peek() == ' ' || file.peek() == '\n') file.ignore();
        if (file.peek() == ';') {
            file >> dispose_char;
            break;
        }
        int a,b,c,d;
        file >> a >> b >> c >> d;
        low_rects.push_back({a,b,c,d});
    }
    /*get all nodes*/
    while (!file.eof()) {
        while (file.peek() == ' ' || file.peek() == '\n') file.ignore();
        if (file.peek() == ';') {
            file >> dispose_char;
            break;
        }
        int a,b;
        file >> a >> b;
        path_nodes.push_back({a,b});
    }
    /*get all node connections*/
    while (!file.eof()) {
        while (file.peek() == ' ' || file.peek() == '\n') file.ignore();
        if (file.peek() == ';') {
            file >> dispose_char;
            break;
        }
        int a,b;
        file >> a >> b;
        path_connections.push_back(Path_Connection{&path_nodes[a],&path_nodes[b]});
    }
    file.close();
}

void save_map_to_file(const std::string& path, std::vector<RECT>& high_rects, std::vector<RECT>& low_rects,
                        std::vector<POINT>& path_nodes, std::vector<Path_Connection>& path_connections) {
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file. Error code: " << strerror(errno) << std::endl;
        return;
    }
    for (RECT& i : high_rects) {
        file << i.left << ' ' << i.top << ' ' << i.right << ' ' << i.bottom << ' ';
    }
    file << ";";
    for (RECT& i : low_rects) {
        file << i.left << ' ' << i.top << ' ' << i.right << ' ' << i.bottom << ' ';
    }
    file << ";";
    for (POINT& i : path_nodes) {
        file << i.x << ' ' << i.y << ' ';
    }
    file << ";";
    for (Path_Connection& i : path_connections) {
        file << i.pt1 - &path_nodes[0] << ' ' << i.pt2 - &path_nodes[0] << ' ';
    }
    file << ";";
    file.close();
}

void load_bitmap(const std::string& path, HBITMAP& bmp) {
    bmp = (HBITMAP) LoadImage(NULL, path.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
}

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = "MOJMENI";                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) CreateSolidBrush(RGB(255,255,255));

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           WS_EX_COMPOSITED | WS_EX_LAYERED,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           _T("Code::Blocks Template Windows App"),       /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           800,                 /* The programs width */
           600,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


enum Action {
    ADD_HIGH_RECT,
    REMOVE_HIGH_RECT,
    ADD_LOW_RECT,
    REMOVE_LOW_RECT,
    ADD_NODE,
    REMOVE_NODE,
    ADD_CONNECTION,
    REMOVE_CONNECTION
};

enum Mode {
    DRAW_HIGH,
    DRAW_LOW,
    DRAW_NODE,
    DRAW_CONNECTION,
    SELECT
};

struct FPOINT {
    double x;
    double y;
};

bool points_equal(const POINT& p1, const POINT& p2) {
    if (p1.x != p2.x) return false;
    if (p1.y != p2.y) return false;
    return true;
}

POINT default_to_screen(const POINT& p, double zoom, const FPOINT& cam_p) {
    POINT ret = p;
    ret.x -= cam_p.x;
    ret.y -= cam_p.y;
    ret.x *= zoom;
    ret.y *= zoom;
    return ret;
}

POINT screen_to_default(const POINT& p, double zoom, const FPOINT& cam_p) {
    POINT ret = p;
    ret.x /= zoom;
    ret.y /= zoom;
    ret.x += cam_p.x;
    ret.y += cam_p.y;
    return ret;
}

RECT default_to_screen(const RECT& r, double zoom, const FPOINT& cam_p) {
    RECT ret = r;
    ret.left -= cam_p.x;
    ret.right -= cam_p.x;
    ret.top -= cam_p.y;
    ret.bottom -= cam_p.y;
    ret.left *= zoom;
    ret.right *= zoom;
    ret.top *= zoom;
    ret.bottom *= zoom;
    return ret;
}

RECT screen_to_default(const RECT& r, double zoom, const FPOINT& cam_p) {
    RECT ret = r;
    ret.left /= zoom;
    ret.right /= zoom;
    ret.top /= zoom;
    ret.bottom /= zoom;
    ret.left += cam_p.x;
    ret.right += cam_p.x;
    ret.top += cam_p.y;
    ret.bottom += cam_p.y;
    return ret;
}

int point_distance_squared(const POINT& p1, const POINT& p2) {
    return (p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y);
}

POINT* get_node_at_point(std::vector<POINT>& nodes, const POINT& point) {
    for (POINT& p : nodes) {
        if (point_distance_squared(p, point) < DEFAULT_NODE_RADIUS * DEFAULT_NODE_RADIUS) return &p;
    }
    return nullptr;
}

bool no_object_under_point(const POINT& point, const std::vector<RECT>& h, const std::vector<RECT>& l, const std::vector<POINT>& p, const std::vector<Path_Connection>& c) {
    return true;
}

void draw_bitmap_scaled(HDC hdc, HBITMAP hbmp, int x, int y, double scale) {
    BITMAP bmp;
    GetObject(hbmp, sizeof(BITMAP), &bmp);
    HDC hdcmem = CreateCompatibleDC(hdc);
    HBITMAP hbmpmem = (HBITMAP) SelectObject(hdcmem, hbmp);

    StretchBlt(hdc, x, y, scale * bmp.bmWidth, scale * bmp.bmHeight, hdcmem, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);

    SelectObject(hdcmem, hbmpmem);
    DeleteDC(hdcmem);
}

/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HBITMAP background_bmp;
    static std::vector<Action> action_history;
    static std::vector<RECT> high_rects;
    static std::vector<RECT> deleted_high_rects;
    static std::vector<RECT> low_rects;
    static std::vector<RECT> deleted_low_rects;
    static std::vector<POINT> path_nodes;
    static std::vector<POINT> deleted_path_nodes;
    static std::vector<Path_Connection> path_connections;
    static std::vector<Path_Connection> deleted_path_connections;

    static Mode current_mode;
    static POINT current_press;
    static RECT current_rect;
    static RECT* selected_rect;
    static POINT* selected_node;
    static Path_Connection* selected_connection;
    static double zoom_value;
    static FPOINT camera_position;
    static bool is_drawing;  //used for high/low objects and path connections while mouse is pressed and the object size is being adjusted
    switch (message)                  /* handle the messages */
    {
        case WM_CREATE:
            current_mode = SELECT;
            zoom_value = 1;
            camera_position.x = camera_position.y = 0;
            is_drawing = false;
            selected_connection = nullptr;
            selected_node = nullptr;
            selected_rect = nullptr;
            break;
        case WM_CHAR:
        {
            switch (wParam) {
            case 'h':
                current_mode = DRAW_HIGH;
                selected_connection = nullptr;
                selected_node = nullptr;
                selected_rect = nullptr;
                is_drawing = false;
                break;
            case 'l':
                current_mode = DRAW_LOW;
                selected_connection = nullptr;
                selected_node = nullptr;
                selected_rect = nullptr;
                is_drawing = false;
                break;
            case 'n':
                current_mode = DRAW_NODE;
                selected_connection = nullptr;
                selected_node = nullptr;
                selected_rect = nullptr;
                is_drawing = false;
                break;
            case 'c':
                current_mode = DRAW_CONNECTION;
                selected_connection = nullptr;
                selected_node = nullptr;
                selected_rect = nullptr;
                is_drawing = false;
                break;
            case 's':
                current_mode = SELECT;
                selected_connection = nullptr;
                selected_node = nullptr;
                selected_rect = nullptr;
                is_drawing = false;
                break;
            case 'u':
                if (action_history.empty()) break;
                Action a = action_history.back();
                action_history.pop_back();
                switch(a) {
                case ADD_HIGH_RECT:
                    high_rects.pop_back();
                    break;
                case ADD_LOW_RECT:
                    low_rects.pop_back();
                    break;
                case ADD_NODE:
                    path_nodes.pop_back();
                    break;
                case ADD_CONNECTION:
                    path_connections.pop_back();
                    break;
                case REMOVE_HIGH_RECT:
                    high_rects.push_back(deleted_high_rects.back());
                    deleted_high_rects.pop_back();
                    break;
                case REMOVE_LOW_RECT:
                    low_rects.push_back(deleted_low_rects.back());
                    deleted_low_rects.pop_back();
                    break;
                case REMOVE_NODE:
                    path_nodes.push_back(deleted_path_nodes.back());
                    deleted_path_nodes.pop_back();
                    break;
                case REMOVE_CONNECTION:
                    path_connections.push_back(deleted_path_connections.back());
                    deleted_path_connections.pop_back();
                    break;
                }
            }
            InvalidateRect(hwnd, 0, 1);
            break;
        }
        case WM_MOUSEMOVE:
        {
            if (wParam == VK_LBUTTON && current_mode == SELECT && no_object_under_point(current_press, high_rects, low_rects, path_nodes, path_connections)) {
                POINT new_mouse;
                new_mouse.x = GET_X_LPARAM(lParam);
                new_mouse.y = GET_Y_LPARAM(lParam);
                camera_position.x -= (new_mouse.x - current_press.x) / zoom_value;
                camera_position.y -= (new_mouse.y - current_press.y) / zoom_value;
                current_press.x = new_mouse.x;
                current_press.y = new_mouse.y;
                if (camera_position.x < 0) camera_position.x = 0;
                if (camera_position.y < 0) camera_position.y = 0;
            }
            else if (is_drawing) {
                current_press.x = GET_X_LPARAM(lParam);
                current_press.y = GET_Y_LPARAM(lParam);
                current_press = screen_to_default(current_press, zoom_value, camera_position);
                current_rect.right = current_press.x;
                current_rect.bottom = current_press.y;
            }
            InvalidateRect(hwnd, 0, 1);
            break;
        }
        case WM_MOUSEWHEEL:
            if (GET_KEYSTATE_WPARAM(wParam) == VK_LBUTTON) break;
            zoom_value += GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA / 4.0;
            if (zoom_value < 0.25) zoom_value = 0.25;
            if (zoom_value > 8) zoom_value = 8;
            InvalidateRect(hwnd, 0, 1);
            break;
        case WM_LBUTTONDOWN:
        {
            if (current_mode == DRAW_NODE) {
                POINT p;
                p.x = GET_X_LPARAM(lParam);
                p.y = GET_Y_LPARAM(lParam);
                p = screen_to_default(p, zoom_value, camera_position);
                path_nodes.push_back(p);
                action_history.push_back(ADD_NODE);
            }
            else if (current_mode == SELECT && no_object_under_point(current_press, high_rects, low_rects, path_nodes, path_connections)) {
                current_press.x = GET_X_LPARAM(lParam);
                current_press.y = GET_Y_LPARAM(lParam);
            }
            else if (current_mode == DRAW_HIGH || current_mode == DRAW_LOW) {
                current_press.x = GET_X_LPARAM(lParam);
                current_press.y = GET_Y_LPARAM(lParam);
                current_press = screen_to_default(current_press, zoom_value, camera_position);
                current_rect.left = current_rect.right = current_press.x;
                current_rect.top = current_rect.bottom = current_press.y;
                is_drawing = true;
            }
            else if (current_mode == DRAW_CONNECTION && (!is_drawing)) {
                current_press.x = GET_X_LPARAM(lParam);
                current_press.y = GET_Y_LPARAM(lParam);
                current_press = screen_to_default(current_press, zoom_value, camera_position);
                selected_node = get_node_at_point(path_nodes, current_press);
                if (selected_node != nullptr) is_drawing = true;
            }
            else if (current_mode == DRAW_CONNECTION && is_drawing) {
                current_press.x = GET_X_LPARAM(lParam);
                current_press.y = GET_Y_LPARAM(lParam);
                current_press = screen_to_default(current_press, zoom_value, camera_position);
                POINT* new_node = get_node_at_point(path_nodes, current_press);
                if (new_node != nullptr && new_node != selected_node) {
                    is_drawing = false;
                    Path_Connection p_c = Path_Connection(selected_node, new_node);
                    bool path_exists = false;
                    for (auto& c : path_connections) if (c == p_c) path_exists = true;
                    if (!path_exists) {
                        path_connections.push_back(p_c);
                        action_history.push_back(ADD_CONNECTION);
                    }
                }
            }
            InvalidateRect(hwnd, 0, 1);
            break;
        }
        case WM_LBUTTONUP:
        {
            if (is_drawing && (current_mode == DRAW_HIGH || current_mode == DRAW_LOW)) {
                is_drawing = false;
                if (current_rect.left == current_rect.right) break;
                if (current_rect.bottom == current_rect.top) break;
                if (current_rect.right < current_rect.left) std::swap(current_rect.left, current_rect.right);
                if (current_rect.bottom < current_rect.top) std::swap(current_rect.bottom, current_rect.top);
                if (current_mode == DRAW_HIGH) {
                    high_rects.push_back(current_rect);
                    action_history.push_back(ADD_HIGH_RECT);
                }
                else if (current_mode == DRAW_LOW) {
                    low_rects.push_back(current_rect);
                    action_history.push_back(ADD_LOW_RECT);
                }
            }
            break;
        }
        case WM_PAINT:
        {
            RECT rect;
            GetClientRect(hwnd, &rect);
            PAINTSTRUCT ps;
            HBRUSH brush;
            HPEN pen;
            HDC hdc = BeginPaint(hwnd, &ps);
            /*DRAWING BACKGROUND BITMAP IF NEEDED*/
            draw_bitmap_scaled(hdc, background_bmp, -camera_position.x * zoom_value, -camera_position.y * zoom_value, zoom_value);
            /*DRAWING NODE POINTS*/
            brush = (HBRUSH) CreateSolidBrush(RGB(255,0,0));
            brush = (HBRUSH) SelectObject(hdc, brush);
            for (POINT p : path_nodes) {
                p = default_to_screen(p, zoom_value, camera_position);
                int r = DEFAULT_NODE_RADIUS * zoom_value;
                Ellipse(hdc, p.x - r, p.y - r, p.x + r, p.y + r);
            }
            DeleteObject(SelectObject(hdc, brush));
            /*DRAWING SELECTED NODE IF NEEDED*/
            if (is_drawing && current_mode == DRAW_CONNECTION) {
                brush = (HBRUSH) CreateSolidBrush(RGB(0,255,0));
                brush = (HBRUSH) SelectObject(hdc, brush);
                POINT p = *selected_node;
                p = default_to_screen(p, zoom_value, camera_position);
                int r = DEFAULT_NODE_RADIUS * zoom_value;
                Ellipse(hdc, p.x - r, p.y - r, p.x + r, p.y + r);
                DeleteObject(SelectObject(hdc, brush));
            }
            /*DRAWING LOW RECTS*/
            brush = (HBRUSH) CreateSolidBrush(RGB(100,100,100));
            for (RECT r : low_rects) {
                r = default_to_screen(r, zoom_value, camera_position);
                FillRect(hdc, &r, brush);
            }
            DeleteObject(brush);
            /*DRAWING HIGH RECTS*/
            brush = (HBRUSH) CreateSolidBrush(RGB(0,0,0));
            for (RECT r : high_rects) {
                r = default_to_screen(r, zoom_value, camera_position);
                FillRect(hdc, &r, brush);
            }
            DeleteObject(brush);
            /*DRAWING CONNECTIONS*/
            pen = (HPEN) CreatePen(PS_SOLID, 1, RGB(255,0,0));
            pen = (HPEN) SelectObject(hdc, pen);
            for (Path_Connection& p_c : path_connections) {
                POINT p1 = *p_c.pt1;
                POINT p2 = *p_c.pt2;
                p1 = default_to_screen(p1, zoom_value, camera_position);
                p2 = default_to_screen(p2, zoom_value, camera_position);
                MoveToEx(hdc, p1.x, p1.y, 0);
                LineTo(hdc, p2.x, p2.y);
                MoveToEx(hdc, 0, 0, 0);
            }
            DeleteObject(SelectObject(hdc, pen));
            /*DRAWING CURRENT RECT IF NEEDED*/
            if (is_drawing && (current_mode == DRAW_HIGH || current_mode == DRAW_LOW)) {
                brush = (HBRUSH) CreateSolidBrush(RGB(200,200,200));
                RECT r = default_to_screen(current_rect, zoom_value, camera_position);
                FillRect(hdc, &r, brush);
                DeleteObject(brush);
            }
            EndPaint(hwnd, &ps);
            break;
        }
        case WM_COMMAND:
        {
            std::string path;
            switch(LOWORD(wParam)) {
            case OPEN_MAPPING:
                path = open_file_omg(hwnd, "Maps(.mp)\0*.MP\0");
                read_and_parse_map(path, high_rects, low_rects, path_nodes, path_connections);
                break;
            case SAVE_MAPPING:
                path = save_file_omg(hwnd, "Maps(.mp)\0*.MP\0");
                save_map_to_file(path, high_rects, low_rects, path_nodes, path_connections);
                break;
            case LOAD_BITMAP:
                path = open_file_omg(hwnd, "Bitmaps(.bmp)\0*.BMP\0");
                load_bitmap(path, background_bmp);
                break;
            }
            InvalidateRect(hwnd, 0, 1);
            break;
        }
        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}
