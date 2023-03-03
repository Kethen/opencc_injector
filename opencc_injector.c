#include <dlfcn.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <opencc.h>
#include <string.h>

static void *_pinyin_handle = NULL;
static void *(*_pinyin_init)(void *system_dir, void *userdir);
static void *(*_pinyin_alloc_instance)(void *context);
static bool (*_pinyin_set_options)(void *context, unsigned int opt);
static void (*_pinyin_free_instance)(void *instance);
static void (*_pinyin_fini)(void *context);
static size_t (*_pinyin_parse_more_full_pinyins)(void *instance, void *pinyin);
static bool (*_pinyin_guess_candidates)(void *instance, size_t offset, unsigned int sort_option);
static bool (*_pinyin_get_n_candidate)(void* instance, unsigned int *num);
static bool (*_pinyin_get_candidate)(void *instance, unsigned int index, void *candidate);
static bool (*_pinyin_get_candidate_string)(void *instance, void *candidate, const char **utf8_str);
static int (*_pinyin_choose_candidate)(void *instance, size_t offset, void* candidate);
static bool (*_pinyin_reset)(void *instance);

opencc_t opencc;

char buffer[1024];

static void init(){
	if(_pinyin_handle){
		return;
	}
	_pinyin_handle = dlopen("/usr/lib/aarch64-linux-gnu/libpinyin.so.13.0.0", RTLD_NOW);
	_pinyin_init = dlsym(_pinyin_handle, "pinyin_init");
	_pinyin_alloc_instance = dlsym(_pinyin_handle, "pinyin_alloc_instance");
	_pinyin_set_options = dlsym(_pinyin_handle, "pinyin_set_options");
	_pinyin_free_instance = dlsym(_pinyin_handle, "pinyin_free_instance");
	_pinyin_fini = dlsym(_pinyin_handle, "pinyin_fini");
	_pinyin_parse_more_full_pinyins = dlsym(_pinyin_handle, "pinyin_parse_more_full_pinyins");
	_pinyin_guess_candidates = dlsym(_pinyin_handle, "pinyin_guess_candidates");
	_pinyin_get_n_candidate = dlsym(_pinyin_handle, "pinyin_get_n_candidate");
	_pinyin_get_candidate = dlsym(_pinyin_handle, "pinyin_get_candidate");
	_pinyin_get_candidate_string = dlsym(_pinyin_handle, "pinyin_get_candidate_string");
	_pinyin_choose_candidate = dlsym(_pinyin_handle, "pinyin_choose_candidate");
	_pinyin_reset = dlsym(_pinyin_handle, "pinyin_reset");

	opencc = opencc_open("s2t.json");
}

void *pinyin_init(void *systemdir, void *userdir){
	if(!_pinyin_handle){
		init();
	}
	return _pinyin_init(systemdir, userdir);
}

void *pinyin_alloc_instance(void *context){
	return _pinyin_alloc_instance(context);
}

bool pinyin_set_options(void *context, unsigned int opt){
	return _pinyin_set_options(context, opt);
}

void pinyin_free_instance(void *instance){
	return _pinyin_free_instance(instance);
}

void pinyin_fini(void *context){
	return _pinyin_fini(context);
}

size_t pinyin_parse_more_full_pinyins(void *instance, void *pinyin){
	return _pinyin_parse_more_full_pinyins(instance, pinyin);
}

bool pinyin_guess_candidates(void *instance, size_t offset, unsigned int sort_option){
	return _pinyin_guess_candidates(instance, offset, sort_option);
}

bool pinyin_get_n_candidate(void* instance, unsigned int *num){
	return _pinyin_get_n_candidate(instance, num);
}

bool pinyin_get_candidate(void *instance, unsigned int index, void *candidate){
	return _pinyin_get_candidate(instance, index, candidate);
}

bool pinyin_get_candidate_string(void *instance, void *candidate, const char **utf8_str){
	bool ret = _pinyin_get_candidate_string(instance, candidate, utf8_str);
	if(ret){
		char* converted = opencc_convert_utf8(opencc, *utf8_str, strlen(*utf8_str));
		if(strlen(converted) >= sizeof(buffer)){
			*utf8_str = NULL;
			opencc_convert_utf8_free(converted);
			return 0;
		}
		*utf8_str = buffer;
		strcpy(buffer, converted);
		opencc_convert_utf8_free(converted);
	}
	return ret;
}

int pinyin_choose_candidate(void *instance, size_t offset, void* candidate){
	return _pinyin_choose_candidate(instance, offset, candidate);
}

bool pinyin_reset(void *instance){
	return _pinyin_reset(instance);
}

// TODO the rest of the functions
// TODO actually fix the thing that uses libpinyin
