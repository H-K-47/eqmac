#ifndef EQDALAYA_HPP
#define EQDALAYA_HPP

#define EVERQUEST_FUNCTION_DO_HOT_BUTTON 0x004F11F9

typedef void __stdcall _everquest_function_do_hot_button(int button_index);
static _everquest_function_do_hot_button *everquest_function_do_hot_button = (_everquest_function_do_hot_button *)EVERQUEST_FUNCTION_DO_HOT_BUTTON;

#define EVERQUEST_FUNCTION_CHAT_WRITE_EX 0x00497105

typedef void __stdcall _everquest_function_chat_write_ex(char* text);
static _everquest_function_chat_write_ex *everquest_function_chat_write_ex = (_everquest_function_chat_write_ex *)EVERQUEST_FUNCTION_CHAT_WRITE_EX;

#endif // EQMAC_HPP
