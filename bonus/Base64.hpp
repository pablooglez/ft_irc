/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Base64.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/07 12:19:56 by albelope          #+#    #+#             */
/*   Updated: 2025/07/15 20:24:33 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BASE64_HPP
#define BASE64_HPP

#include <string>

class Base64
{
		public:
			static std::string encodeBase64(const std::string &input);
			static std::string decodeBase64(const std::string &input);
};

#endif