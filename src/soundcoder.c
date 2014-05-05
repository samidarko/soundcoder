#include <Python.h>
#include <lame/lame.h>
#include <faac.h>

static char soundcoder_wav_to_mp3_doc[] = "wav to mp3"; 
static PyObject* soundcoder_wav_to_mp3(PyObject* self, PyObject* args)
{
    PyObject * pyfile;
 
    if (!PyArg_ParseTuple(args, "O", &pyfile)) {
        return NULL;
    }

    if (!PyFile_Check(pyfile)) {
	PyErr_SetString(PyExc_IOError, "Argument must be Python file object");
	return NULL;
    }
    FILE *pcm  = PyFile_AsFile( pyfile );

    // int read, write;
    int read, bufSize;
    const int PCM_SIZE = 8192;
    const int MP3_SIZE = 8192;
    short int pcm_buffer[PCM_SIZE*2];
    unsigned char mp3_buffer[MP3_SIZE];

    PyObject * lst = PyList_New(0);
    if (lst == NULL) {
	PyErr_SetString(PyExc_Exception, "Can not create Python list");
	return NULL;
    }
    PyObject* tmpstr;

    lame_t lame = lame_init();
    lame_set_in_samplerate(lame, 44100);
    lame_set_VBR(lame, vbr_default);
    lame_init_params(lame);

    do {
        read = fread(pcm_buffer, 2*sizeof(short int), PCM_SIZE, pcm);
        if (read == 0)
            bufSize = lame_encode_flush(lame, mp3_buffer, MP3_SIZE);
        else 
            bufSize = lame_encode_buffer_interleaved(lame, pcm_buffer, read, mp3_buffer, MP3_SIZE);
        
	    tmpstr = Py_BuildValue("s#", mp3_buffer, bufSize);
    	    PyList_Append(lst, tmpstr);
    	    Py_DECREF(tmpstr);
    } while (read != 0);

    return lst;
}

static char soundcoder_wav_to_aac_doc[] = "wav to aac"; 
static PyObject* soundcoder_wav_to_aac(PyObject* self, PyObject* args)
{
    PyObject * pyfile;
 
    if (!PyArg_ParseTuple(args, "O", &pyfile)) {
        return NULL;
    }

    if (!PyFile_Check(pyfile)) {
	PyErr_SetString(PyExc_IOError, "Argument must be Python file object");
	return NULL;
    }
    FILE *pcm  = PyFile_AsFile( pyfile );

    PyObject * lst = PyList_New(0);
    if (lst == NULL) {
	PyErr_SetString(PyExc_Exception, "Can not create Python list");
	return NULL;
    }
    PyObject* tmpstr;

    unsigned long sampleRate = 44100;
    unsigned int numChannels = 2;
    unsigned long inputSamples;
    unsigned long maxOutputBytes;

    faacEncHandle hEncoder = faacEncOpen(sampleRate, numChannels, &inputSamples, &maxOutputBytes);
    if (!hEncoder) {
	PyErr_SetString(PyExc_Exception, "Can not open Faac encoder");
        return NULL;
    }

    faacEncConfigurationPtr faacConfig = faacEncGetCurrentConfiguration(hEncoder);
    faacConfig->inputFormat = FAAC_INPUT_16BIT;
    faacConfig->bitRate = 64000;
    if (!faacEncSetConfiguration(hEncoder, faacConfig)) {
	PyErr_SetString(PyExc_Exception, "Can not apply Faac configuration");
        return NULL;
    }

    int32_t pcm_buffer[inputSamples];
    unsigned char data[maxOutputBytes];
    int bufSize;

    while (fread(pcm_buffer, sizeof(short int), inputSamples, pcm)) {
        bufSize = faacEncEncode(hEncoder, pcm_buffer, inputSamples, data, maxOutputBytes);
	tmpstr = Py_BuildValue("s#", data, bufSize);
    	PyList_Append(lst, tmpstr);
    	Py_DECREF(tmpstr);
    }

    while(bufSize != 0){
        bufSize = faacEncEncode(hEncoder, NULL, 0, data, maxOutputBytes);
	tmpstr = Py_BuildValue("s#", data, bufSize);
    	PyList_Append(lst, tmpstr);
    	Py_DECREF(tmpstr);
    }
    
    faacEncClose(hEncoder);

    return lst;
}

static PyMethodDef module_methods[] = {
     {"wav_to_mp3", soundcoder_wav_to_mp3, METH_VARARGS, soundcoder_wav_to_mp3_doc},
     {"wav_to_aac", soundcoder_wav_to_aac, METH_VARARGS, soundcoder_wav_to_aac_doc},
     {NULL, NULL, 0, NULL}
};
 
PyMODINIT_FUNC initsoundcoder(void)
{
     (void) Py_InitModule3("soundcoder", module_methods, "A fast sound encoder");
}
