
#ifndef HAORANBCQ_H
#define HAORANBCQ_H

#include <ac_std_float.h>
#include <systemc.h>
#include <nvhls_module.h>
#include <ac_float.h>
#include <ac_fixed.h>
#include <nvhls_marshaller.h>
#include <ac_channel.h>
#include <nvhls_connections.h>
#include <ac_math/ac_pow_pwl.h>
#include <mem_array.h>

typedef ac_ieee_float<binary16> fp16_hr;
typedef ac_ieee_float<binary32> fp32_hr;

typedef fp16_hr INT4XPre;

class msg_INT4Vector: public nvhls_message {
public:
    INT4XPre datax[32];
    NVUINT32 dataw[8];

    static unsigned int const width = 32*INT4XPre::width + 8*NVUINT32::width;

    template <unsigned int Size>
    void Marshall(Marshaller<Size> &m) {
        for (int i = 0; i < 32; i++) {
            m& datax[i];
        }
        for (int i = 0; i < 8; i++) {
            m& dataw[i];
        }
    }   
};

class BaselineINT4Dequant: public match::Module {
    SC_HAS_PROCESS(BaselineINT4Dequant);
public:
    Connections::In<msg_INT4Vector> Input_INT4Vector;
    Connections::Out<INT4XPre> Output_INT4XPre;

    BaselineINT4Dequant(sc_module_name name) : match::Module(name) {
        SC_THREAD(BaselineDequant);
        sensitive << clk.pos();
        async_reset_signal_is(rst, false);
    }

    void BaselineDequant() {
        Input_INT4Vector.Reset();
        Output_INT4XPre.Reset();

#pragma hls_pipeline_init_interval 1
        while (1) {
            wait();
            if (!Input_INT4Vector.Empty()) {
                msg_INT4Vector req = Input_INT4Vector.Pop();

                NVUINT8 dataw_decompress_fromNVUINT32[32];
                #pragma hls_unroll
                for (int i = 0; i < 8; i++) {
                    #pragma hls_unroll
                    for (int j = 0; j < 4; j++) {
                        dataw_decompress_fromNVUINT32[4*i + 3 - j] = (req.dataw[i] >> (j)) & 1;
                    }
                }
                // std::cout << "data x: ";
                // for (int i = 0; i < 32; i++) {
                //     std::cout << req.datax[i] << " ";
                // }
                // std::cout << std::endl;
                // std::cout << "dataw_decompress_fromNVUINT32: ";
                // for (int i = 0; i < 32; i++) {
                //     std::cout << dataw_decompress_fromNVUINT32[i] << " ";
                // }
                // std::cout << std::endl;
                // exit(0);
                
                INT4XPre layer1[32];
                #pragma hls_unroll
                for (int i = 0; i < 32; i++) {
                    layer1[i] = INT4XPre(dataw_decompress_fromNVUINT32[i]) * INT4XPre(req.datax[i]);
                }
                INT4XPre layer2[16];
                #pragma hls_unroll
                for (int i = 0; i < 16; i++) {
                    layer2[i] = layer1[2*i] + layer1[2*i+1];
                }
                INT4XPre layer3[8];
                #pragma hls_unroll
                for (int i = 0; i < 8; i++) {
                    layer3[i] = layer2[2*i] + layer2[2*i+1];
                }
                INT4XPre layer4[4];
                #pragma hls_unroll
                for (int i = 0; i < 4; i++) {
                    layer4[i] = layer3[2*i] + layer3[2*i+1];
                }
                INT4XPre layer5[2];
                #pragma hls_unroll
                for (int i = 0; i < 2; i++) {
                    layer5[i] = layer4[2*i] + layer4[2*i+1];
                }
                INT4XPre layer6[1];
                layer6[0] = layer5[0] + layer5[1];
                Output_INT4XPre.Push(layer6[0]);
            }
        }
    }
};

#endif