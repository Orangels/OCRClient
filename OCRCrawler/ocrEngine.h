//#ifndef  _PPADDLE_OCR_H
//#define _PPADDLE_OCR_H
#pragma once
#include <string>
#include <vector>
#include "opencv2/core.hpp"

//#ifdef OCR_DLL
//#define OCR_DLL_EXPORT_IMPORT __declspec(dllexport)  
//#else  
//#define OCR_DLL_EXPORT_IMPORT __declspec(dllimport)  
//#endif  


//lass OCR_DLL_EXPORT_IMPORT  ocrEngine
class   ocrEngine
{
	public:
		ocrEngine();
		~ocrEngine();
		struct OCRConfig {
			bool use_gpu;
			std::string det_model_path;
			std::string rec_model_path;
			std::string rec_dict_path;
			float  score_threshold;
		};
		struct OCRResult {
			std::vector<std::vector<int>> box;
			std::string text;
			bool  used;
		};
		static ocrEngine  getInstance() {
			static ocrEngine  instance;
			return instance;
		};
		void initialize(OCRConfig ocrconfig);
		std::vector<std::string> detectfile(std::string imgFile);
		std::vector <ocrEngine::OCRResult>  detectdata(cv::Mat imgdata);
		void release();
};
//#endif