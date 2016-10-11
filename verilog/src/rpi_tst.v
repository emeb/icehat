// lattice ice5lp4k spi_slave + R G B blinky for RPi
// 10-05-16 E. Brombaugh

module rpi_tst(
	// SPI slave port
	input SPI_CSL,
	input SPI_MOSI,
	output SPI_MISO,
	input SPI_SCLK,
	
	// RGB output
	output wire o_red,
	output wire o_green,
	output wire o_blue
);

	// This should be unique so firmware knows who it's talking to
	parameter DESIGN_ID = 32'h1CE50100;

	//------------------------------
	// Instantiate HF Osc with div 1
	//------------------------------
	wire clk;
	SB_HFOSC #(.CLKHF_DIV("0b00")) OSCInst0 (
		.CLKHFEN(1'b1),
		.CLKHFPU(1'b1),
		.CLKHF(clk)
	) /* synthesis ROUTE_THROUGH_FABRIC= 0 */;
	
	//------------------------------
	// reset generator
	//------------------------------
	reg [3:0] reset_pipe = 4'hf;
	reg reset = 1'b1;
	always @(posedge clk)
	begin
		reset <= |reset_pipe;
		reset_pipe <= {reset_pipe[2:0],1'b0};
	end
	
	//------------------------------
	// Internal SPI slave port
	//------------------------------
	wire [31:0] wdat;
	reg [31:0] rdat;
	wire [6:0] addr;
	wire re, we, spi_slave_miso;
	spi_slave
		uspi(.clk(clk), .reset(reset),
			.spiclk(SPI_SCLK), .spimosi(SPI_MOSI),
			.spimiso(SPI_MISO), .spicsl(SPI_CSL),
			.we(we), .re(re), .wdat(wdat), .addr(addr), .rdat(rdat));
	
	//------------------------------
	// Writeable registers
	//------------------------------
	reg [13:0] cnt_limit_reg;
	reg [31:0] freq;
	reg gate;
	always @(posedge clk)
		if(reset)
		begin
			cnt_limit_reg <= 14'd2499;	// 1/4 sec blink rate
		end
		else if(we)
			case(addr)
				7'h01: cnt_limit_reg <= wdat;
			endcase
	
	//------------------------------
	// readback
	//------------------------------
	always @(*)
		case(addr)
			7'h00: rdat = DESIGN_ID;
			7'h01: rdat = cnt_limit_reg;
			default: rdat = 32'd0;
		endcase
	
	//------------------------------
	// Instantiate LF Osc
	//------------------------------
	wire CLKLF;
	SB_LFOSC OSCInst1 (
		.CLKLFEN(1'b1),
		.CLKLFPU(1'b1),
		.CLKLF(CLKLF)
	) /* synthesis ROUTE_THROUGH_FABRIC= 0 */;
	
	//------------------------------
	// Divide the clock
	//------------------------------
	reg [13:0] clkdiv;
	reg onepps;
	always @(posedge CLKLF)
	begin		
		if(clkdiv == 14'd0)
		begin
			onepps <= 1'b1;
			clkdiv <= cnt_limit_reg;
		end
		else
		begin
			onepps <= 1'b0;
			clkdiv <= clkdiv - 14'd1;
		end
	end
	
	//------------------------------
	// LED signals
	//------------------------------
	reg [2:0] state;
	always @(posedge CLKLF)
	begin
		if(onepps)
			state <= state + 3'd1;
	end
	
	//------------------------------
	// Instantiate RGB DRV 
	//------------------------------
	wire red_pwm_i = state[0];
	wire grn_pwm_i = state[1];
	wire blu_pwm_i = state[2];
	SB_RGB_DRV RGB_DRIVER (
	   .RGBLEDEN  (1'b1), // Enable current for all 3 RGB LED pins
	   .RGB0PWM   (red_pwm_i), // Input to drive RGB0 - from LEDD HardIP
	   .RGB1PWM   (grn_pwm_i), // Input to drive RGB1 - from LEDD HardIP
	   .RGB2PWM   (blu_pwm_i), // Input to drive RGB2 - from LEDD HardIP
	   .RGBPU     (led_power_up_i), //Connects to LED_DRV_CUR primitive
	   .RGB0      (o_red), 
	   .RGB1      (o_green),
	   .RGB2      (o_blue)
	);
	defparam RGB_DRIVER.RGB0_CURRENT = "0b000111";
	defparam RGB_DRIVER.RGB1_CURRENT = "0b000111";
	defparam RGB_DRIVER.RGB2_CURRENT = "0b000111";

	//------------------------------
	// Instantiate LED CUR DRV 
	//------------------------------
	SB_LED_DRV_CUR LED_CUR_inst (
		.EN    (1'b1), //Enable to supply reference current to the LED drivers
		.LEDPU (led_power_up_i) //Connects to SB_RGB_DRV primitive
	);

endmodule
