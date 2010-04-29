       R0.L=0xc2;
        R1.L=0x1c;
        P0.L=0x90;
        P0.H=0x100;
        R3.L=R0.L+R1.L (NS);
        W[P0]=R3.L;
        SSYNC;
        RTS;
