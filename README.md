# Telegram-RAT
 
 This is a telegram RAT i maid for the NEOLAND Bootcamp final proyect.
 It has the most basic functionality and i did it for learning.

 ## Commands

 - **/activate**: For activating a machine.
    - **ip**: machine IP
    - **all**: activate all machines

- **/shell**: Opens a shell for executing commands in the machine(s). Once the shell is opened, you send the commands without '/'. Example: cd, whoami
- **/screenshot**: Takes a screenshot of the machine.
- **/process**: Takes to posible arguments:
    - **list**: for listing the processes currently running in the machine:
    - **inject_shell**: for injecting shellcode into a process. Usage: **/process inject_shell <process_name>**. For example: **/process inject_shell notepad.exe**. After that, you need to send the .bin file with the shellcode you want to inject


## Requirements

I used the tgbot.cpp API: https://github.com/reo7sp/tgbot-cpp.

You can install it using **vcpkg** ( if i rememeber correctly ). 

You can install the static or the dynamic one, the dynamic one its going to generate the dlls that tgbot needs in the compilation. If you dont want to have dlls, you should install the static libraries with: `vcpkg install tgbot-cpp:x64-windows-static`.

Im pretty sure that the command was like that but you can always ask CHAT-GPT :). 

I would test it myself but im running linux and i can't install everything right now in my windows VM.



