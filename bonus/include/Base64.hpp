/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Base64.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/07 12:19:56 by albelope          #+#    #+#             */
/*   Updated: 2025/07/09 17:16:20 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

std::string encodeBase64(const std::string &input);
std::string decodeBase64(const std::string &input);