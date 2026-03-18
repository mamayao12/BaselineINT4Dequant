//
// Created by sli941 on 9/14/2024.
//

#define NVHLS_VERIFY_BLOCKS (BaselineINT4Dequant)

#include "BaselineINT4Dequant.h"
#include <nvhls_verify.h>
#include <random>
#include <systemc.h>
#include "ac_sysc_trace.h"

// #include <mc_verify.h>

class testbench : public sc_module {
public:
    sc_clock clk;
    sc_signal<bool> rst;

    Connections::Combinational<msg_INT4Vector> Input_BaselineDequant;
    Connections::Combinational<INT4XPre> Output_BaselineDequant;

    NVHLS_DESIGN(BaselineINT4Dequant) dut;

    SC_CTOR(testbench) : clk("clk", 1, SC_NS, 0.5, 0, SC_NS, true),
                         rst("rst"),
                         Input_BaselineDequant("Input_BaselineDequant"),
                         Output_BaselineDequant("Output_BaselineDequant"),
                         dut("dut") {

        sc_object_tracer<sc_clock> trace_clk(clk);

        dut.clk(clk);
        dut.rst(rst);
        dut.Input_INT4Vector(Input_BaselineDequant);
        dut.Output_INT4XPre(Output_BaselineDequant);

        SC_THREAD(reset);
        sensitive << clk.posedge_event();

        SC_THREAD(run);
        sensitive << clk.posedge_event();
        async_reset_signal_is(rst, false);

        SC_THREAD(collect);
        sensitive << clk.posedge_event();
        async_reset_signal_is(rst, false);
    }

    void reset() {
        rst.write(false);
        wait(10);
        rst.write(true);
    }

    void run() {
        Input_BaselineDequant.ResetWrite();
        wait(10);

        ifstream infile_INT4Vector;
        
        
        const char * env_p = std::getenv("EIC_SERVER");
        if (env_p == nullptr) {
            std::cout << "EIC_SERVER not set, using default path" << std::endl;
            infile_INT4Vector.open("/Users/lisixu/Projects/EICHW/A1_cmod/CATEXP/HaoranBCQ/testcases/INT4Vector.txt");
        } else {
            std::cout << "EIC_SERVER set to " << env_p << std::endl;
            infile_INT4Vector.open("/home/sli941/Project/YangMeeting/cmod/BaselineINT4Dequant/INT4Vector.txt");
        }

        if (!infile_INT4Vector.is_open()) {
            std::cerr << "infile_INT4Vector open failed" << std::endl;
            sc_stop();
        }
        
        

        // read file
        // For int4vector input, first read 32 integers as datax, then read 32 integers as dataw, then read 2 floats as scaling factor
        // For haoran input, first read 12 * 8 floats as inputx, then read 12 integers as inputw
        // For int4vector output, read 1 float as outputPsum
        // For haoran output, read 3 floats as outputPsum
        int tmp_int;
        float tmp_float;
        msg_INT4Vector INT4Vector_Input[128 * 3];

        for (int i = 0; i < 128; i++) {
            for (int k = 0; k < 3; k++) {
                for (int j = 0; j < 32; j++) {
                    infile_INT4Vector >> tmp_float;
                    INT4Vector_Input[i * 3 + k].datax[j] = INT4XPre(tmp_float);
                }
                for (int j = 0; j < 8; j++) {
                    infile_INT4Vector >> tmp_int;
                    INT4Vector_Input[i * 3 + k].dataw[j] = NVUINT32(tmp_int);
                }
            }
        }

        // send data to dut
        // first send data to baselineDequant
        for (int i = 0; i < 128; i++) {
            for (int k = 0; k < 3; k++) {
                Input_BaselineDequant.Push(INT4Vector_Input[i * 3 + k]);
                wait();
            }
        }
    }

    void collect() {

        Output_BaselineDequant.ResetRead();
        
        
        ifstream infile_INT4Vector_gold;
        const char * env_p = std::getenv("EIC_SERVER");
        if (env_p == nullptr) {
            std::cout << "EIC_SERVER not set, using default path" << std::endl;
            infile_INT4Vector_gold.open("/Users/lisixu/Projects/EICHW/A1_cmod/CATEXP/HaoranBCQ/testcases/INT4VectorGold.txt");
        } else {
            std::cout << "EIC_SERVER set to " << env_p << std::endl;
            infile_INT4Vector_gold.open("/home/sli941/Project/YangMeeting/cmod/BaselineINT4Dequant/INT4VectorGold.txt");
        }
        if (!infile_INT4Vector_gold.is_open()) {
            std::cerr << "infile_INT4Vector_gold open failed" << std::endl;
            sc_stop();
        }

        int tmp_int;
        float tmp_float;
        INT4XPre INT4Vector_Output[128];

        INT4XPre errorlimit = INT4XPre(0.01);

        for (int i = 0; i < 128; i++) {
            infile_INT4Vector_gold >> tmp_float;
            INT4Vector_Output[i] = INT4XPre(tmp_float);
        }

        bool DequantPass = true;
        for (int i = 0; i < 123; i++) {
            INT4XPre outputPsum1 = Output_BaselineDequant.Pop();
            INT4XPre outputPsum2 = Output_BaselineDequant.Pop();
            INT4XPre outputPsum3 = Output_BaselineDequant.Pop();
            INT4XPre outputPsum = outputPsum1 + outputPsum2 + outputPsum3;
            if ((outputPsum - INT4Vector_Output[i]).abs() > errorlimit) {
                DequantPass = false;
                std::cout << "Dequant mismatch at: " << i << " expected: " << INT4Vector_Output[i] << " actual: " << outputPsum << " @ " << sc_time_stamp() << std::endl;
            }
        }
        if (DequantPass) {
            std::cout << "Dequant pass at " << sc_time_stamp() << std::endl;
        }
        
        sc_stop();
    }
};

int sc_main(int argc, char *argv[]) {
    testbench tb("tb");
    sc_start();
    return 0;
}
