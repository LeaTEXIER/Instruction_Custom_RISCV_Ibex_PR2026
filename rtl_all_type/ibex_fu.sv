module ibex_fu
(
 input ibex_pkg::fu_op_e    operator_i,
 input  logic [31:0]        operand_a_i,
 input  logic [31:0]        operand_b_i,
 output logic [31:0]        result_o,
 output logic               valid_o
);
    import ibex_pkg::*;
    logic [4:0] shamt;
    logic [31:5] unused_bits = operand_b_i[31:5];

    always_comb begin
        result_o  	= '0;
        valid_o   	= 1'b0;
        
        shamt		= operand_b_i[4:0];

        unique case (operator_i)
            FU_MOD: begin
                result_o = operand_a_i % operand_b_i; //32'h12345678;//operand_a_i % operand_b_i; //MODIF
                valid_o   = 1'b1;
            end
            FU_MROLI: begin
                result_o = (operand_a_i >> shamt) | (operand_a_i << (32 - shamt));
                valid_o   = 1'b1;
            end
            FU_MDROLI: begin
             result_o = (operand_b_i >> 7) | (operand_b_i << (25));
                valid_o   = 1'b1;
            end 
            FU_JUMB1: begin
                result_o = (operand_a_i ~^ operand_b_i); //NXOR
                valid_o   = 1'b1;
            end
            FU_JUMB2: begin
                result_o = operand_a_i + operand_b_i;
                valid_o   = 1'b1;
            end
            default: begin
            	result_o = 32'h20030717;
                valid_o   = 1'b0;
            end
        endcase
    end
endmodule
