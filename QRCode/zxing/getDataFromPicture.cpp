/*
* Copyright 2016 Nu-book Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include <ZXing/ReadBarcode.h>
#include <ZXing/TextUtfEncoding.h>
#include <ZXing/BarcodeFormat.h>
#include <ZXing/DecodeStatus.h>

#include <iostream>
#include <cstring>
#include <string>
#include <algorithm>
#include <cctype>

#define STB_IMAGE_IMPLEMENTATION
#include "./thirdparty/stb/stb_image.h"

using namespace ZXing;

std::ostream& operator<<(std::ostream& os, const Position& points) {
	for (const auto& p : points)
		os << p.x << "x" << p.y << " ";
	return os;
}

int main()
{
	// Decoderの設定
	DecodeHints hints;
	hints.setTryHarder(false);

	// 読み込む画像
	std::string filePath = "./qr.png";

	int width, height, channels;
	std::unique_ptr<stbi_uc, void(*)(void*)> buffer(stbi_load(filePath.c_str(), &width, &height, &channels, 4), stbi_image_free);
	
	if (buffer == nullptr) {
		std::cerr << "Failed to read image: " << filePath << "\n";
		return -1;
	}

	auto result = ReadBarcode({buffer.get(), width, height, ImageFormat::RGBX}, hints);
	std::cout << "test is " << TextUtfEncoding::ToUtf8(result.text()) << std::endl;
	
	return 0;
}
