/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Base64.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albelope <albelope@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/07 12:19:56 by albelope          #+#    #+#             */
/*   Updated: 2025/07/07 12:22:33 by albelope         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BASE64_HPP
#define BASE64_HPP

#include <string>

std::string encodeBase64(const std::string &input);
std::string decodeBase64(const std::string &input);


#endif