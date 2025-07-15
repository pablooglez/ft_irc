/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Base64.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/07 12:19:59 by albelope          #+#    #+#             */
/*   Updated: 2025/07/15 20:24:06 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Base64.hpp"
#include "../include/Client.hpp"

static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
										"abcdefghijklmnopqrstuvwxyz"
										"0123456789+/";

std::string Base64::encodeBase64(const std::string &input)
{
	std::string		stringToEncode;
	unsigned char	originalBytes[3];
	unsigned char	base64Indexs[4];
	int				countByte = 0;

	for (size_t i = 0; i < input.size(); ++i)
	{
		originalBytes[countByte++] = input[i];
		
		if (countByte == 3)
		{
			base64Indexs[0] = (originalBytes[0] & 0xfc) >> 2;
			base64Indexs[1] = ((originalBytes[0] & 0x03) << 4) + ((originalBytes[1] & 0xf0) >> 4);
			base64Indexs[2] = ((originalBytes[1] & 0x0f) << 2) + ((originalBytes[2] & 0xc0) >> 6);
			base64Indexs[3] = originalBytes[2] & 0x3f;

			for (int j = 0; j < 4; j++)
				stringToEncode += base64_chars[base64Indexs[j]];

			countByte = 0;
		}
	}

	if (countByte > 0)
	{
		for (int j = countByte; j < 3; j++)
			originalBytes[j] = '\0';

		base64Indexs[0] = (originalBytes[0] & 0xfc) >> 2;
		base64Indexs[1] = ((originalBytes[0] & 0x03) << 4) + ((originalBytes[1] & 0xf0) >> 4);
		base64Indexs[2] = ((originalBytes[1] & 0x0f) << 2) + ((originalBytes[2] & 0xc0) >> 6);
		base64Indexs[3] = originalBytes[2] & 0x3f;

		for (int j = 0; j < countByte + 1; j++)
			stringToEncode += base64_chars[base64Indexs[j]];

		int paddingCount = 3 - countByte;
		for (int j = 0; j < paddingCount; j++)
			stringToEncode += '=';
	}

	return stringToEncode;
}

static int getBase64Index(char c)
{
	if (c >= 'A' && c <= 'Z')
		return (c - 'A');
	if (c >= 'a' && c <= 'z')
		return (c - 'a' + 26);
	if (c >= '0' && c <= '9')
		return (c - '0' + 52);
	if (c == '+')
		return (62);
	if (c == '/')
		return (63);
	return (-1);
}

std::string Base64::decodeBase64(const std::string &input)
{
	std::string stringToDecode;
	unsigned char base64Chars[4];
	unsigned char originalBytes[3];
	int countChar = 0;

	for (size_t i = 0; i < input.size(); ++i)
	{
		char currentChar = input[i];

		if (currentChar == '=' || currentChar == ' ' || currentChar == '\n' || currentChar == '\r')
			continue;

		int index = getBase64Index(currentChar);
		if (index == -1)
			continue;

		base64Chars[countChar++] = index;

		if (countChar == 4) {
			originalBytes[0] = (base64Chars[0] << 2) + ((base64Chars[1] & 0x30) >> 4);
			originalBytes[1] = ((base64Chars[1] & 0x0f) << 4) + ((base64Chars[2] & 0x3c) >> 2);
			originalBytes[2] = ((base64Chars[2] & 0x03) << 6) + base64Chars[3];

			for (int j = 0; j < 3; j++)
				stringToDecode += originalBytes[j];

			countChar = 0;
		}
	}

	if (countChar > 0)
	{
		for (int j = countChar; j < 4; j++)
			base64Chars[j] = 0;

		originalBytes[0] = (base64Chars[0] << 2) + ((base64Chars[1] & 0x30) >> 4);
		originalBytes[1] = ((base64Chars[1] & 0x0f) << 4) + ((base64Chars[2] & 0x3c) >> 2);
		originalBytes[2] = ((base64Chars[2] & 0x03) << 6) + base64Chars[3];

		if (countChar == 2)
			stringToDecode += originalBytes[0];
		else if (countChar == 3)
		{
			stringToDecode += originalBytes[0];
			stringToDecode += originalBytes[1];
		}
	}
	return (stringToDecode);
}
