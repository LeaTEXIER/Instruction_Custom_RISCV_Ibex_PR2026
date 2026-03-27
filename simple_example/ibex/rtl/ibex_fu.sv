// Copyright lowRISC contributors.
// Copyright 2018 ETH Zurich and University of Bologna, see also CREDITS.md.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

/**
 * Arithmetic logic unit
 */
module ibex_fu #(
) (
  input  ibex_pkg::fu_op_e operator_i,
  input  logic [31:0]       operand_a_i,
  input  logic [31:0]       operand_b_i,

  output logic [31:0]       result_o,
  output logic fu_use
);
    import ibex_pkg::*;
    logic [31:5] unused_bits = operand_b_i[31:5];
    logic [4:0] shamt;
    logic [4:0] test;

    always_comb begin
        result_o = '0;
        fu_use = 0;
        
        shamt = operand_b_i[4:0];
        test = shamt;
        shamt = test;
        if (operator_i != 0) begin
            $display("result = %h", operator_i);
        end
        unique case (operator_i)

            FU_MROLI: begin
              result_o = (operand_a_i >> shamt) | (operand_a_i << (32 - shamt));
// result_o = 32'hDEADBEEF;
                $display("result = %h et entrée = %h", result_o, operand_a_i);
                fu_use = 1'b1;
                // result_o = operand_a_i % operand_b_i;
            end

            default: result_o = '0;

        endcase
    end
endmodule

