# Raulisr00t Ransomware

This program is a basic ransomware example that demonstrates the following functionalities:

1. **Retrieves the path to the user's Documents folder.**
2. **Finds all `.txt` files within the Documents folder.**
3. **Encrypts these files using XOR encryption with a predefined key.**
4. **Self-deletes the executable after performing the operations.**

## Features

- **GetDocumentsPath**: Retrieves the path to the user's Documents folder.
- **GetTxtFilesInDirectory**: Lists all `.txt` files in the specified directory.
- **XorEncryptDecrypt**: Encrypts or decrypts data using XOR with a specified key.
- **EncryptFile**: Encrypts a file's content using XOR encryption.
- **SelfDelete**: Deletes the executable file after encryption.

## Usage

1. **Compile the Program**: Use a C++ compiler to build the executable.
2. **Run the Executable**: Execute the program to encrypt all `.txt` files in the Documents folder and then delete itself.

**Note**: This program is intended for educational purposes only. Use it responsibly and do not deploy it in a real-world scenario without proper precautions.

## Disclaimer

This code is provided for educational and informational purposes only. Do not use it for malicious activities. Unauthorized use of ransomware is illegal and unethical. The authors and contributors of this code do not endorse or support any illegal activities.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
