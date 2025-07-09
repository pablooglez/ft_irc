/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Base64.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/07 12:19:59 by albelope          #+#    #+#             */
/*   Updated: 2025/07/09 17:12:34 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/Base64.hpp"


//	me toca en examen y me crujen mijo
// tabla base64, cada posicion es un simbolo que representa un valor entre 0 y 63
static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
										"abcdefghijklmnopqrstuvwxyz"
										"0123456789+/";

std::string encodeBase64(const std::string &input) {
	std::string		stringToEncode;
	unsigned char	originalBytes[3];	// guardar 3 bytes que vamos leyendo del string original
										// base64 trabaja en bloques de 3 bytes para convertirlos en 4 simbolos
	unsigned char	base64Indexs[4];	// guardar los 4 indices que vamos a sacar de esos 3 bytes
										// esos indices nos dicen que letras de la tabla base64 vamos a usar
	int				countByte = 0;		// contador to saber cuantos bytes hemos leido y save en el array originalBytes

	
	for (size_t i = 0; i < input.size(); ++i) {			// recorremos todo el string original, byte a byte, uno por uno
		originalBytes[countByte++] = input[i];			// guardamos el byte que estamos leyendo en el array y sumamos 1 al contador
		
		if (countByte == 3)								// cuando tenemos 3 bytes completos, podemos convertirlos a base64
		{
			base64Indexs[0] = (originalBytes[0] & 0xfc) >> 2;			// cogemos los 6 bits altos del primer byte
			base64Indexs[1] = ((originalBytes[0] & 0x03) << 4) + ((originalBytes[1] & 0xf0) >> 4); // cogemos 2 bits del primero y 4 bits del segundo
			base64Indexs[2] = ((originalBytes[1] & 0x0f) << 2) + ((originalBytes[2] & 0xc0) >> 6); // cogemos 4 bits del segundo y 2 bits del tercero
			base64Indexs[3] = originalBytes[2] & 0x3f;			// cogemos los 6 bits bajos del tercer byte

			for (int j = 0; j < 4; j++)							// recorremos los 4 indices base64
				stringToEncode += base64_chars[base64Indexs[j]]; // guardamos el caracter base64 que corresponde en el string final

			countByte = 0;										// reiniciamos el contador de bytes para el siguiente bloque
		}
	}

	if (countByte > 0)											// si quedan bytes sueltos al final (menos de 3)
	{
		for (int j = countByte; j < 3; j++)						// rellenamos con ceros los bytes que faltan hasta tener 3
			originalBytes[j] = '\0';							// ponemos ceros donde no tenemos datos

		base64Indexs[0] = (originalBytes[0] & 0xfc) >> 2;		// calculamos los indices base64 igual que antes
		base64Indexs[1] = ((originalBytes[0] & 0x03) << 4) + ((originalBytes[1] & 0xf0) >> 4);
		base64Indexs[2] = ((originalBytes[1] & 0x0f) << 2) + ((originalBytes[2] & 0xc0) >> 6);
		base64Indexs[3] = originalBytes[2] & 0x3f;

		for (int j = 0; j < countByte + 1; j++)					// guardamos solo los simbolos validos que hemos calculado
			stringToEncode += base64_chars[base64Indexs[j]];

		int paddingCount = 3 - countByte;						// calculamos cuantos '=' necesitamos
		for (int j = 0; j < paddingCount; j++)					// añadimos padding '=' para completar 4 caracteres
			stringToEncode += '=';								// siempre tiene que quedar un bloque de 4 caracteres
	}

	return stringToEncode;										// devolvemos el string final ya codificado en base64
}


// funcion para buscar la posicion de un caracter en la tabla base64
static int getBase64Index(char c) {
    if (c >= 'A' && c <= 'Z')
		return (c - 'A');            		// si es mayuscula, le restamos 'A'
    if (c >= 'a' && c <= 'z')
		return (c - 'a' + 26);      		// si es minuscula, le restamos 'a' y sumamos 26 posiciones
    if (c >= '0' && c <= '9')
		return (c - '0' + 52);       		// si es numero, le restamos '0' y sumamos 52 posiciones
    if (c == '+')
		return (62);                      // si es '+', su indice es 62
    if (c == '/')
		return (63);                      // si es '/', su indice es 63
    return (-1);                                         
}

std::string decodeBase64(const std::string &input) {
    std::string stringToDecode;                          // aqui guardamos el resultado final decodificado
    unsigned char base64Chars[4];                        // guardar 4 caracteres base64 que vamos leyendo
    unsigned char originalBytes[3];                      // guardar los 3 bytes originales que obtenemos
    int countChar = 0;                                   // contador de caracteres base64 leidos

    for (size_t i = 0; i < input.size(); ++i) {          // recorremos todo el string base64
        char currentChar = input[i];                     // guardamos el caracter actual

        if (currentChar == '=' || currentChar == ' ' || currentChar == '\n' || currentChar == '\r') // ignoramos padding y espacios
            continue;

        int index = getBase64Index(currentChar);         // buscamos la posicion del caracter en la tabla base64
        if (index == -1)
			continue;                      				 // si no es valido, lo ignoramos

        base64Chars[countChar++] = index;                // guardamos el indice en el array y sumamos 1 al contador

        if (countChar == 4) {                            // cuando tenemos 4 caracteres base64 completos
            originalBytes[0] = (base64Chars[0] << 2) + ((base64Chars[1] & 0x30) >> 4); // reconstruimos el primer byte original
            originalBytes[1] = ((base64Chars[1] & 0x0f) << 4) + ((base64Chars[2] & 0x3c) >> 2); // reconstruimos el segundo byte original
            originalBytes[2] = ((base64Chars[2] & 0x03) << 6) + base64Chars[3]; // reconstruimos el tercer byte original

            for (int j = 0; j < 3; j++)                  // guardamos los 3 bytes originales en el string final
                stringToDecode += originalBytes[j];

            countChar = 0;                               // reiniciamos el contador de caracteres base64
        }
    }

    if (countChar > 0) {                                 // si quedan caracteres base64 sueltos al final
        for (int j = countChar; j < 4; j++)              // rellenamos con ceros los que faltan hasta tener 4
            base64Chars[j] = 0;

        originalBytes[0] = (base64Chars[0] << 2) + ((base64Chars[1] & 0x30) >> 4); // reconstruimos el primer byte original
        originalBytes[1] = ((base64Chars[1] & 0x0f) << 4) + ((base64Chars[2] & 0x3c) >> 2); // reconstruimos el segundo byte original
        originalBytes[2] = ((base64Chars[2] & 0x03) << 6) + base64Chars[3]; // reconstruimos el tercer byte original

        if (countChar == 2)                              // si quedan 2 caracteres base64 ➜ solo hay 1 byte valido
            stringToDecode += originalBytes[0];
        else if (countChar == 3)                         // si quedan 3 caracteres base64 ➜ hay 2 bytes validos
        {
            stringToDecode += originalBytes[0];
            stringToDecode += originalBytes[1];
        }
        												// si solo queda 1 caracter base64, lo ignoramos, es invalido
    }

    return (stringToDecode);                            // devolvemos el string final decodificado
}
