#include <stdio.h>
#include <string.h>
#include <iostream>
#include <malloc.h>
using namespace std;


char* MyTrim(const char* text) {
	char* res = nullptr;
	size_t resLength = 0;
	size_t countSpaces = 0;

	__asm {
		//====================================================================================================================================================
		mov ecx, text //копируем указатель на строку в регистр ecx
		xor eax, eax  //быстрое зануление регистра eax // eax считает длинну строки без учёта пробелов в конце
		xor ebx, ebx  //ebx = 0 //ebx нужен чтобы считать пробелы после не пробелов
		//====================================================================================================================================================

		cycleCountSpaces: // запускаем цикл, где считаем колличество символов в строке, не учитывая последние пробелы, если пробелы не последние, прибавим их к общему числу символов

		mov dl, [ecx]     //смотрим что лежит по адресу ecx text[i]
		cmp dl, 0         //сравниваем символ с нулём
		je ExCycleCountSpaces // если равны, то это значит, что мы дошли до конца строки и нужно из неё выйти

        inc ecx               //ecx++

		cmp dl, ' '           // вравниваем символ строки с пробелом
		je addValueToSpaceCounter // если равны, то ebx ++
		jne addValueToResLength  // если не равны, то eax++ eax = eax+ebx ebx = 0

		returnToCycleCountSpaces:

		jmp cycleCountSpaces 
		//====================================================================================================================================================

		addValueToSpaceCounter: 
		inc ebx                        //ebx++
		jmp returnToCycleCountSpaces
		//====================================================================================================================================================

		addValueToResLength:
		inc eax                        //eax++
		add eax, ebx                   //eax = eax+ebx
		xor ebx, ebx                   //ebx = 0
		jmp returnToCycleCountSpaces
		//====================================================================================================================================================

		ExCycleCountSpaces:            // вышли из цикла
		mov resLength, eax            // как и говорилось, сохраняем длинну будущей строки res 
		inc eax                        // готовим eax как аргумент для функции, которая выделит память, поскольку в конце символ 0, под который тоже нужно выделить память, то eax++

		push eax
		call malloc                    // для выделения памяти используем функцию из языка С res = (char*)malloc(resLength+1)
		add esp, 4

		
		//====================================================================================================================================================
		mov res, eax                   // malloc возвращает указательна память, которую она выделила, указатель лежит в регистре eax// res = eax 
		mov ecx, text                  // копируем указатель на строку в регистр ecx

		// следующие 3 строки написаны, чтобы проверить, является ли строка text пустой
		mov ebx, resLength            
		cmp ebx, 0                     // сравниваем ebx = resLength с нулём
		je EXIT                        // если длинна строки равна нулю, то не будем ничего больше делать и выйдем из строки

		xor ebx, ebx                   //ebx = 0

		// если так получилось, что входная строка не пустая, то копируем строки в цикле
		Cycle:                         //for( ebx = 0; ebx < resLength; ebx++){
		mov dl, [ecx + ebx]            // dl = text[i]
		mov [eax], dl                  // res[i] = dl

		inc eax                  //передвинем указатель на начало массива res в право
		inc ebx                  //передвинем указатель на начало массива text в право
		cmp ebx, resLength       // сравним ebx и resLength 
		je EXIT                  // если равны, то выйти из программы

		jmp Cycle                //}
		//====================================================================================================================================================
		EXIT:
		mov [eax], 0             // программа написана так, что когда мы оказываемся в метке EXIT  то eax всегда ссылается на конец строки, поэтому запишем туда 0
	}
	return res;
}
int main() {
	// тест с идеальный случаем                              "qwert         ",  "qwert"      +
	// тест где пробел не встречается в строке               "qwert",           "qwert"      +
	// тест где пробел встречается не однократно             "qw  er   t     ", "qw  er   t" +                 
	// тест на пустую строку                                 "",                ""           + 

	//cout << "start string>" << MyTrim("qw  er   t") << "<end string";

	const char enterText[] = "Please enter text:\n";
	const char endString[] = "<end string\n";
	const char outputResult[] = "Get the result:\n";
	const char repeatEnter[] = "\nWant to re-enter?\n"
		"(Enter)-repeat\n";
	const char scanfStr[] = "%[^\n]s";

	char inputText[100];
	char inputSymbol[2];
	char keepWorkingOrNot[1];
	__asm {
	REPEATE:
		lea eax, enterText           //
		push eax                     //
		call printf_s                // printf_s(enterText)
		add esp, 4                   //

		mov eax, 100                 //get_s(inputText, 100)
		push    eax                  //
		lea     eax, inputText       //
		push	eax                  //
		call    gets_s               //
		add esp, 8                   //


		lea eax, outputResult        //
		push eax                     //
		call printf_s                // printf_s(outputResult)
		add esp, 4                   //

		mov dl, byte ptr[inputSymbol]//
		push edx                     //
		lea eax, inputText           //
		push eax                     //MyTrim(inputText)
		call MyTrim                  //
		add esp, 8                   //

		push eax                     //
		call printf_s                // print_f(MyTrim(inputText))
		add esp, 4                   //

		lea eax, endString
		push eax                     //
		call printf_s                // print_f(MyTrim(inputText))
		add esp, 4                   //

		lea eax, repeatEnter         //
		push eax                     //
		call printf_s                // printf_s(repeatEnter)
		add esp, 4                   //

		mov eax, 1                   //
		push eax                     //
		lea eax, keepWorkingOrNot    //
		push eax                     //
		call gets_s                  //get_s(keepWorkingOrNot, 1)
		add esp, 8

		mov dh, [keepWorkingOrNot]   // выбираем между тем, чтобы закончить работу программы или повторить операцию
		cmp dh, 0                    //
		je REPEATE
		jne EXIT
	EXIT:
	}
}