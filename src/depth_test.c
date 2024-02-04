#include <stdio.h>
#include <math.h>

#include "board.h"
#include "eval.h"
#include "depth_test.h"	

const char* FEN_POSITIONS[500] = {
	"r4rk1/ppp1qpbp/2n2np1/3pNb2/3Pp3/2N1P1PP/PPPQ1PB1/R1B2RK1",
	"2r3k1/3n1pp1/3R1p1p/1b1p4/3P3P/4PNP1/1P3PB1/6K1",
	"rn1q2k1/Nppb1rb1/3p2n1/1P1Pp1pp/4Pp2/3N1P2/P3BBPP/2RQ1RK1",
	"r7/1p3k2/6pb/p1p1PP1n/2Pp2p1/6P1/PP1B1RK1/4R3",
	"4k2r/2b3pp/r3p3/p2P4/1ppP2Pn/2P4P/PP3RBq/R1BQ2K1",
	"r1bqr1k1/pppn1pbp/6p1/3p4/3P4/2P1P1B1/PPBQ1PPP/R4RK1",
	"r4rk1/pp3pb1/7p/2p3pP/1n4P1/1P1p2N1/PBPP1P2/R2K3R",
	"r2q1rk1/1b2ppbp/pn4p1/1p1p4/3P1N2/P1P1P1PP/3N1P2/R2QKB1R",
	"2r2rk1/pp2qpp1/2pp3p/7P/3pP3/8/PPP2P2/R2Q1K1R",
	"r1bqk2r/p2pnpbp/2p3p1/4pP2/4P1P1/2N5/PPP4P/R1BQKB1R",
	"5k2/6r1/2P5/pK1P4/1b6/1B6/P7/8",
	"r1q3k1/p3b1pp/np2p3/3pN3/2pP4/2P3P1/PPQN2PP/5RK1",
	"1r3r2/k1q1n1p1/2nbP3/pRp3Np/P2pQP1P/3P2P1/3B2B1/2R3K1",
	"r1b1k2r/4bpp1/1ppppn1p/8/4P3/P1N5/1PP1BPPP/R2Q1RK1",
	"1r4k1/3R1pp1/6p1/1p2n3/p2N4/P1P4P/1P3PP1/6K1",
	"7r/p6p/B2kbp2/4p3/8/P4N2/1Pr2PPP/R5K1",
	"r2qr1k1/ppp2ppp/2n5/3n4/3P2b1/P3PN2/1B2BPPP/R2QK2R",
	"r1bqk2r/1ppp1ppp/p7/3nP3/1bBp4/3P4/PPPN1PPP/R1BQK2R",
	"8/8/2P5/b2R4/k7/8/1K6/8",
	"r1b3k1/pp1n1r1p/2p2q1B/4bp2/2Pnp3/Q1N3P1/PP3PBP/3RNRK1",
	"8/2k2pp1/1p2p3/p3PP2/b2P2P1/8/3B4/6K1",
	"6k1/1p1N1pb1/p6p/8/r5p1/2n3B1/5PPP/4R1K1",
	"r2bB1k1/pp3pp1/7p/8/1P6/PQ3P1b/3q1P1P/6RK",
	"r1bq1rk1/ppp1bpp1/7p/2p1N3/8/2Q4P/PPP2PP1/RNB2RK1",
	"r1bqkb1r/ppp1n1pp/2n2p2/3Bp3/4PP2/5N2/PPPP2PP/RNBQK2R",
	"1k1r4/ppp1R3/8/5prp/P2R4/2n3P1/2P2P1P/6K1",
	"r3B1k1/p4pp1/3b1nbp/2p5/2Pp1Q1B/5N2/PP3PPP/R5K1",
	"r2r2k1/1pp2pp1/pn2q3/4n2p/3N4/Q1P1P1P1/PP2BPP1/R2R2K1",
	"rnbqkbnr/1ppp1pp1/4p3/8/2PPP2p/P4N1P/P4PP1/RNBQKB1R",
	"rn1q1rk1/1b1p1ppp/p3pb2/1pp5/5P1P/1P1P2P1/PBPNQP2/2KR2R1",
	"Q1bq1knr/4bppp/1p1p4/2p1p3/4P3/8/PP1PNPPP/RNB2RK1",
	"r5k1/pp2bp2/2n2p1p/4p3/4P2q/1P1P1Q2/1PPN2PP/R4R1K",
	"r2k1b1r/p4pp1/8/5n2/p1P2P1p/5p1P/8/2B1K3",
	"rnbqkbnr/1ppp1p2/p3p3/4N2p/3P1Pp1/4P3/PPP3PP/RNBQKB1R",
	"3R4/2K1kp2/6p1/p4PPp/Pp5P/8/8/8",
	"r1b1k2r/pp1p1ppp/1qn2n2/4p3/2p1P3/2NP1NP1/PPP3BP/R1BQK2R",
	"4r3/ppp3pp/6k1/2b1n3/4P3/P1NR1r2/1P4PP/5R1K",
	"1r4k1/5pp1/3p1r1p/1P1Rp3/2P1Q3/5PPP/q3B1K1/3R4",
	"r5k1/1p3ppp/p4b2/2p5/2Pp4/1B4P1/PP2q3/R5K1",
	"3q1r2/ppk2p2/2p1p2Q/2PnPp2/7P/6P1/PP3P2/3RR1K1",
	"rn1qkb1r/ppp2ppp/8/2nP4/4p3/3P1Q1P/PP3PP1/RNB1KB1R",
	"r2qkb1r/ppp2ppp/4b3/n2n4/4Q3/3B4/PPP2PPP/RNB1K1NR",
	"8/1p5p/6k1/3N4/1B1P2p1/8/PPP2R1P/R6K",
	"r2qkb1r/pp3p1p/2p2np1/3p1b2/3P4/2N1PQ1P/PPP2P2/R3KBNR",
	"6k1/5p1p/4p1p1/3p3P/4n3/P4rP1/1R6/4R1K1",
	"r7/pppn1kb1/6p1/3p4/3P1B1q/2P2P1b/PP2NP1P/RN2K2R",
	"3r1r2/4bk2/pq1p1p1B/1p1R3Q/2n1P3/P6P/1P4P1/2R4K",
	"8/8/4K3/8/8/8/2k5/qq6",
	"5R2/4r3/4k3/p5B1/8/Pp6/1P6/R6K",
	"3q1rk1/pp1b2p1/2p3n1/3p2p1/3Nn3/1BPQ4/PP4PP/R3R1K1",
	"2kr3r/ppp2pp1/3b1n2/7p/4PP2/2N3Pq/PP1BQ2P/R4RK1",
	"8/3qk2p/1Q4p1/2p2r2/2P5/4P3/P5PP/6K1",
	"3r2k1/p4ppp/1p6/2p5/4B1P1/1P2BP2/1KP4P/8",
	"r1bqkb1r/p2n1ppp/2p2n2/1p2p3/3P4/1B2PN2/PP3PPP/RNBQ1RK1",
	"8/1p3pkp/4p1p1/2P5/RpN5/1P5P/5PP1/3r2K1",
	"5k2/R7/p1p3p1/Pp5p/1P4P1/8/7K/4R3",
	"r4rk1/1p2bppp/1np1pn2/4N3/3PQP2/PN2P2R/6PP/R1B3K1",
	"1r3rk1/1ppb1pp1/p2p1q1p/3PpP2/2Bb4/P1PP1Q1P/1P4P1/R1B2R1K",
	"rnbq1rk1/1p2ppbp/p2p1np1/2pP4/2P1P3/3BBP2/PPQ3PP/RN2K1NR",
	"2kr1b1r/ppp2pp1/2n2q1p/8/PP2p1b1/2P2NP1/3NPPBP/R2QK2R",
	"8/p1p4p/1p1r1k2/6p1/1Rn1Pp2/8/P1P1BKPP/8",
	"4k1B1/ppp1b1Pp/8/8/8/5K2/P4PPP/8",
	"6k1/2B3p1/p1p2p2/6p1/4P3/8/PPP3PP/2K5",
	"2bk2nr/1p1p4/r6p/p2Q4/P7/N4PP1/8/R3KBNR",
	"8/5K1k/5PR1/7q/1p6/1B6/P7/8",
	"5rk1/p4p2/2p2bpp/5b2/8/1P1Q1N1P/P5PK/1B6",
	"rnb1k2N/ppp2pp1/8/2b1p1p1/2B5/5P2/PPP3PP/RN2K2R",
	"r2kbQ2/ppp5/3p1p2/3Pp3/2n1Pq2/2P1R3/P1P4P/2K1R3",
	"rn1q1rk1/ppp2pbp/3pbnp1/4p3/4P3/1PN2NP1/PBPP1PBP/R2QK2R",
	"8/1p3p1p/1k3p2/1P6/7P/r5r1/6K1/1R6",
	"4r1k1/pp4p1/2pb1qb1/3p2N1/7P/8/P4PP1/R2Q2K1",
	"r4r1k/1b4p1/p2pQ2p/2p5/8/2N3KP/PPP3P1/4R3",
	"4r1k1/5pp1/p2B3p/1p6/5P2/8/PPP3PP/3R2K1",
	"2bqk1r1/rpnp1pp1/p1p1pn2/8/P6P/P2PP2R/1BP2PP1/1N1QKB2",
	"7k/pp2r1p1/7P/2p3R1/3b3P/7r/Pn6/1R5K",
	"rn2k2r/ppq3p1/2pbp3/7p/6B1/7P/PPPP1PP1/RNBQR1K1",
	"r1bq1rk1/p3bppp/n1pp1n2/1p2p3/4P1P1/1BNP1N1P/PPP2P2/R1BQK2R",
	"2rq1rk1/4bppp/p2p4/nnp1pP2/4P3/2PP1N1P/P1BB1P2/R2QR1K1",
	"r5k1/p1p3pp/2nq2r1/1pNn1p2/8/1P1PPQ1P/PB3PP1/R4RK1",
	"N7/pp2kp2/3p2rn/1b6/3P3K/2P3P1/PP4P1/4RR2",
	"2rr2k1/R3np2/6pp/3PR3/8/7N/P1p2PPP/2Q3K1",
	"8/8/n4Pk1/3BK3/1p6/8/2P4p/8",
	"r2qr1k1/ppp2ppp/2n2n2/2bpp3/4P1b1/2PP1NP1/PP1N1PBP/R1BQR1K1",
	"r3k2r/pp3ppp/8/2p5/K3N1b1/PP1P2P1/5b1P/2q2B1R",
	"r4rk1/2p2p2/p2p1Ppq/1p2pb2/4Pn2/2PP2R1/PP3QPP/3B1RK1",
	"8/7Q/3p4/1p3k2/4b2P/2P2P2/PP4r1/2KR3R",
	"r2qk2r/1b3pp1/n1ppN1np/1p6/p3P2b/P2PBN1P/BPPQ1PP1/R3K2R",
	"r5k1/8/8/PP1p1r2/3B2p1/4K3/5P2/R7",
	"8/4k3/p3p3/1p3p2/1Pp2P1r/2B2R2/P1P5/2K5",
	"3r2k1/R4p2/1B3bpp/8/4p3/2r3P1/P1P3KP/8",
	"5rk1/ppp3pp/2n1b3/4p3/2B4N/2P1P3/P1Pr1RPP/R5K1",
	"r2qkb2/p1p2pp1/5n2/3pp1Br/2B5/5P2/PPP4P/RN1QK1R1",
	"r1b1kbnr/ppp3pp/8/5p2/3np2q/2N5/PPPN1PBP/R1BQK2R",
	"r1bqkbnr/pppp1ppp/2n5/4p3/4P3/P4Q2/1PPP1PPP/RNB1KBNR",
	"4rr2/1pk3pp/p5b1/P3P2P/1p6/4K3/8/8",
	"r4b1r/4kp2/1N1ppnpp/8/3p1PP1/4B3/PPP1P2P/R2QK1NR",
	"3r1rk1/1pq4p/6pQ/p4p2/2bn4/P1B2P1B/1PK1P3/6R1",
	"r1bqk2r/1ppn1pbp/p5p1/1N1pP2n/8/5N1P/PPP1PPPB/R2QKB1R",
	"8/r5kp/1p2p1p1/p2r1p2/P2Pp3/1RP4P/1P3PP1/4R1K1",
	"r1b2rk1/p2n1ppp/1pn1p3/q2pP3/1b1P4/P1NBBN2/1P1Q1PPP/R4RK1",
	"rnb1qrk1/2p2pbp/1pP1p1p1/pP1pP1B1/3P3P/P1NB2P1/5P2/R2Q1RK1",
	"rr6/p1k2ppp/2n3q1/1QNp3n/5p2/8/PPP3PP/R1B2RK1",
	"r3k2r/ppqn1pbp/3p1np1/3Pp1B1/4P1b1/2N2N1P/PP2BPP1/R2Q1RK1",
	"r3k2r/pp3ppp/8/3Pn3/1bq5/4K2N/PP3PPP/R2Q3R",
	"2k5/8/1pr5/p5PK/8/8/8/8",
	"5rk1/1ppnb1p1/p1p4p/6P1/3PPB1Q/5P1K/PPP5/RN3R2",
	"5r1k/p1B1b1pp/4Pp2/N2q1P2/b1pp4/2p3Q1/PrP3PP/1RR3K1",
	"3r1k1r/p4p2/2p1pb1P/1pq5/8/P3P3/1P2QP2/R1B1K2R",
	"r2qkb1r/ppp3pp/2nppn2/4p3/4P3/2N2N2/PPPP1PPP/R1BQR1K1",
	"rnbq1rk1/pppp1pbp/6p1/8/4n3/4P1P1/PPPN1PBP/RNBQ1RK1",
	"r1bqk2r/p1pn1p1p/4pp2/1p4B1/2pP4/P1P5/5PPP/R2QKBNR",
	"3K4/3R4/8/8/p6P/P3p3/1q2k3/8",
	"r4rk1/pp1bqppp/2p1p3/3nP3/3PN1P1/1B6/PPP1Q2P/1K1R3R",
	"r2q1rk1/p4ppp/1ppb1n2/8/2QP4/4PNB1/PP3PPP/R3K2R",
	"R7/pk1P3p/1p6/1Np1N1P1/8/6PP/PPb3P1/3R2K1",
	"5rk1/1q1bnpbp/p3p1p1/Pp1pP3/1P1P1P2/1Q2B3/3NN1PP/2r2RK1",
	"5rk1/pp2rppp/8/P1pp4/7q/1P6/3B2B1/1N3RK1",
	"8/8/p7/P7/4k3/5pK1/1r6/6B1",
	"2qr2k1/1r3pbp/6p1/3p1b2/8/1BQ3B1/2P1nPPP/1R1NR1K1",
	"2k1rb1r/2q3p1/p1bppp2/6Bp/2R1PP1P/1P4Q1/P1P1N1P1/2K4R",
	"r2qkb1r/p1p3pp/b4p2/4pN2/2Pp2Q1/NP6/P2P1PPP/R1B2RK1",
	"r1r3k1/p1pbp2p/2p3p1/Q1P2p2/3PP3/4Pq2/PP4RP/R3B2K",
	"8/8/P3k3/8/1K6/1P6/8/3R4",
	"4r3/pp1n2pk/2pb1p1p/3q3b/3Pr3/1P1Q4/PB4P1/2R2RK1",
	"rnbq1rk1/p1ppppbp/1p4p1/3n4/3P4/3BPNB1/PPP2PPP/RN1QK2R",
	"r3r1k1/ppq2ppp/2n5/4P2n/8/2B2Q2/PPP2PPP/R3KB1R",
	"5r1k/ppp3p1/3p1q1p/4p3/8/1BP3PQ/PPP3KP/4R3",
	"4r1k1/pp3p1p/6p1/8/1P2p1Q1/P6P/1q2KPP1/3R4",
	"r1bq1rk1/ppp2ppp/1Nnp1n2/4p3/2B1P3/3P1N2/PPP2PPP/R1BQ1RK1",
	"r1bqk2r/p3bppp/1pn1pn2/2pp2B1/3P4/2PBPN2/PP1N1PPP/R2Q1RK1",
	"r1bqkb1r/pppp2pp/2n2p2/3nP3/8/N1P2Q2/PP3PPP/R1B1KBNR",
	"5rk1/ppp3pp/2p5/5p2/7q/4P1P1/PPPPQ2P/R1B2rK1",
	"8/pQ1K4/8/8/P2P4/4k3/8/8",
	"rnbqkbnr/ppp1pp1p/8/3p2p1/2P3P1/8/PP1PPPBP/RNBQK1NR",
	"rn3rk1/ppb1npp1/2p2q2/4p3/2B1P1b1/2N2N2/PPP1QP1P/1K1R2R1",
	"7r/4bk1p/2p5/4np2/P3pp2/2P1P3/4BPPP/3R2K1",
	"rn1qk2r/pbpp1ppp/1p2pn2/8/2PPP3/2P1BP2/P5PP/R2QKBNR",
	"2RQ1b1r/kp3pp1/p3q3/3pP2p/3P4/4BbP1/PP5P/4R1K1",
	"r1b5/ppp5/3p4/2k1P3/2q5/2N1Q3/PP3PPP/R3K2R",
	"6r1/3n1R2/p2p4/kP1Pp3/4P3/4K3/Pq3Q2/8",
	"8/6R1/4pk2/7R/1P6/1r6/6P1/6K1",
	"r4r1k/1bq2ppp/p1n2P2/1pp1p1BQ/P2pP3/3P4/BPP3PP/R4R1K",
	"rnbqkbnr/4pppp/pppp4/8/2PPPB2/2N5/PP3PPP/R2QKBNR",
	"rn1qkb1r/3b1ppp/3ppN2/2p3B1/ppB1P3/3P1N2/PPP2PPP/R2QK2R",
	"rnbqk2r/pppp1ppp/3b1n2/4P3/5p2/2N2N2/PPPP2PP/R1BQKB1R",
	"rn1qkb1r/pp3ppp/2p1pn2/3p4/3P4/2P1P1B1/PPb1BPPP/RN2K1NR",
	"4K3/5pk1/1q4p1/6Pp/6r1/4n3/8/8",
	"8/3R1pk1/4p1pp/rB2P1P1/P4P1P/4PK2/8/2r5",
	"rn1q1rk1/3b1ppp/p2bpn2/1pp5/3P4/2NBPN2/PPQ2PPP/R1B2RK1",
	"8/p1b5/1p4p1/1B2p2p/P3K2P/4P3/3k4/8",
	"8/5ppk/5nrp/8/2R4P/5P2/5P1K/8",
	"N4q1k/pp4p1/2p4p/4Qb2/1PB5/P2P4/2P3PP/R1B3K1",
	"2kr3r/3qbppp/2bp1n2/p1p1pPB1/1P2P2Q/3P1N2/1PP1N1PP/R4R1K",
	"r1bq1rk1/p3bppp/2n1pn2/1ppp4/4P3/3P1NP1/PPPN1PBP/R1BQR1K1",
	"8/8/8/5ppk/3K1bb1/2R5/8/8",
	"8/pp3kpp/2p1p3/7P/P2P2P1/1Pb2NK1/8/3r2B1",
	"3r2k1/5bpp/3p4/6Pn/4P3/3PK3/8/6R1",
	"2rqr1k1/pp3p1p/6pP/2n2P2/3Np3/2P1P3/PPK1Q3/6RR",
	"r5k1/ppp2pp1/5n1p/3N1b2/5B2/5N2/PP4PP/3RrK2",
	"r7/pp1r1p1p/7k/8/8/6R1/PR6/7K",
	"r1bq1rk1/ppp2ppp/2n2n2/4p3/1b2P3/1BNP1N2/PPP3PP/R1BQK2R",
	"8/p5p1/1p2k2p/8/1R3P2/8/KP3PPP/8",
	"r1bq1r1k/pp4pp/2n2p2/4pP1Q/4B3/2P5/P1P4P/R1B2RK1",
	"r2qk2r/ppp1bppp/2n1pn2/6B1/2BPp1b1/2N3N1/PPPQ1PPP/R3K2R",
	"r2qr1k1/pp3ppp/1p6/3pn3/3Pn1b1/P1BQ4/4BPPP/R3K2R",
	"4r3/p7/4pkbB/2b5/2B2pPp/1P6/P4P2/3R2K1",
	"rn3r2/1ppb2bk/p2p2pp/3Pp3/2P4q/2N1B3/PPB1QPPP/R3R1K1",
	"r2qr1k1/ppp2p1p/6pb/4npN1/8/3P4/PPP2PPP/R1BQR1K1",
	"r4rk1/ppp1qpp1/2p1bn2/6Bp/3bP3/3P2QP/PPP1NPP1/4RRK1",
	"rn1qkbnr/pp3ppp/2pp4/4p3/4P1b1/5NP1/PPPP1PBP/RNBQ1RK1",
	"r4rk1/1pp2pp1/pbb1pnqp/8/8/2P1BQ2/PP3PPP/R3R1K1",
	"r3k2r/1pp1np2/p2p2q1/P2Pp3/4P2p/2P1PNpB/1P1N4/R2QK3",
	"1rbq1rk1/p3pp1p/6p1/3p4/3b4/5P2/PPPQ2PP/2KR1B1R",
	"r3kbnr/pppbq1pp/2n1p3/1B1pPp2/3P4/5N2/PPP2PPP/RNBQ1RK1",
	"4qrk1/p1R1brpp/1p1pQ2B/4p3/4P3/8/PP3PPP/2R3K1",
	"1k6/p1p5/3p4/4p3/2P1P3/1P1Pr1Kp/P7/R7",
	"4r3/5bpk/2pB1p1p/p1P5/2P2R2/P6P/2P2KP1/8",
	"r1q2rk1/1p1n1pp1/p1nBp2p/8/4P3/1Q4P1/PP3PBP/2RR2K1",
	"8/p3ppk1/3p3p/2pP1Pp1/1rN3P1/2b1PK1P/2R5/8",
	"2k2b1r/pp2pppp/2n2q2/1Q1N4/8/5P2/PP2B2P/1K5R",
	"r1bqkb1r/p2p1ppp/2n1p3/4P3/NpPp4/1P3Q2/PB3PPP/R3KB1R",
	"3r2k1/p1p2p1p/1p4p1/3r1n2/1P6/7P/P5P1/R3R2K",
	"r1b2r1k/pp4pp/5q2/2N2p2/3P4/1Q2PR2/PP4PP/R5K1",
	"6k1/p4qpp/2pb2b1/1pn5/3Br3/1P5Q/P1P1N1PP/4R1K1",
	"5rk1/6p1/4p1pp/3p1q2/3P4/3Q4/5RPP/7K",
	"1r2kb1r/2q1pnpp/p1p1bp2/2p5/2Pp1PP1/1P1P1N1P/P3Q3/RNB2RK1",
	"r2qkb1r/pppb1ppp/4pn2/1B1pP3/8/4P3/PPP2PPP/RNBQK2R",
	"r1q1kb1r/pbpp1pp1/1p2pnnp/4P1N1/2B5/2NP4/PPP1QPPP/R1B1K2R",
	"8/5ppk/p1R4p/1p2K3/4P1P1/7P/8/r7",
	"r1b1k2r/pp1nbp2/2pp1n2/7q/3PPNpp/1NP3P1/PP3PBP/R1B1QRK1",
	"3r4/p1p1kppp/8/1p1Bb3/8/1P6/P5PP/R1B2RK1",
	"1n1r3r/1k5p/p4R2/1P5R/4P3/3P2P1/1P4K1/8",
	"rn1qk1nr/pp1B1ppp/4p3/2bp4/4P3/5N2/PPP2PPP/RNBQK2R",
	"r2qk2r/p2n1ppp/2Q1p3/1N1p1b2/2PPn3/8/PP1KPPPP/R4BNR",
	"r2r2k1/bppn2p1/p3bp1p/4p3/4P3/1PP2NP1/PB3PBP/R3R1K1",
	"8/Kn1k2p1/8/5p2/8/8/1r6/7q",
	"8/2Q3pp/3Bkr2/8/8/2P5/PP3PPP/6K1",
	"3k1r2/p6p/3P4/1R6/8/8/3RK3/2r5",
	"3q2k1/1pr1npp1/p3p2p/3pPbQ1/B6B/P7/5PPP/2R1R1K1",
	"2r3k1/5ppp/1p2p3/p1p1P3/1r6/1PR2P1P/5P2/2R2K2",
	"r2qkb1r/pp3ppp/2n1bn2/2ppp1N1/4P3/P1PP2P1/1P3P1P/RNBQKB1R",
	"r3r3/p4pkp/1pp2qp1/3p3b/3P1P2/7B/PP1Q1R1R/2K5",
	"rnbqkb1r/pp1ppppp/8/3nP3/3p4/2P5/PP3PPP/RNBQKBNR",
	"r4rk1/pp1n2pp/2p1pqb1/3n4/2BP1PQ1/8/PPP3PP/R1B2RK1",
	"8/8/6p1/R7/6pk/8/p5K1/r7",
	"8/B4Pk1/B3Q2p/6p1/N7/8/3K1PPP/8",
	"4R3/2pk1ppp/8/r7/2p5/p4P2/5KPP/2r5",
	"r3nrk1/pppqbp2/2npb1p1/4p3/4P3/1BNPBN2/PPPQ1PPP/R4RK1",
	"1r2r1k1/R4pp1/1p5p/1P6/2Ppq3/3R2P1/5PNP/1Q4K1",
	"r1bqkbnr/pp3ppp/2n5/8/3Np3/2N5/PP3PPP/R1BQKB1R",
	"Qn2r1k1/p1p2ppp/8/3P4/b7/1BP5/P1KPq1PP/R1B3bR",
	"2r2r2/1pq2pk1/p3p1pp/2P5/2pPbP2/P1B1P2R/1Q4PP/5RK1",
	"r2q1rk1/1bp1ppbp/pn1p2p1/4P2n/3P4/2NB1N1P/1PP2PPB/R2QK2R",
	"r3k1nr/1pp2ppp/p1pb4/4p3/4P3/3PBP2/PPP2P1P/RN3RK1",
	"5rk1/5ppp/2rNp3/q2pP3/1p1BnP2/8/2PQ2PP/5RK1",
	"2r1k2r/1pq2ppp/p3bn2/2b1p3/4PP2/2NB4/PP4PP/R1BQ1R1K",
	"r1bqk2r/pp2ppb1/2np1np1/2p4p/4PPP1/2NP1N1P/PPP5/R1BQKB1R",
	"2k5/p1pp4/8/1r3p1p/2r5/1KN2b1P/PPP5/R7",
	"8/b5B1/P5p1/3k1p1p/P4P2/3K3P/8/8",
	"8/8/2p3p1/1pPk1bPp/1K5P/8/6B1/8",
	"8/5k2/1R3p2/5n2/5N2/4r3/5K2/8",
	"r1bqk2r/pp3ppp/1n6/3pp3/2P5/2P5/P3BPPP/R1BQ1RK1",
	"1k2rr2/pP6/3p2pb/q1pN1P1p/4P3/5QN1/1PP5/1K3R2",
	"r2q1rk1/ppp3p1/4pbQ1/8/8/2N5/PPP3PP/R1B3K1",
	"r5k1/6pp/5p2/p7/1N6/1PB5/P4PPP/3r2K1",
	"1r3b1r/p1k2p2/2p4p/6p1/1P1P4/2P1N3/P4PPP/RN1R2K1",
	"1kr5/pp6/2p3QB/2Pp4/8/8/PPP2qP1/1K1RR3",
	"rn3rk1/ppb2p2/7R/4p3/8/1N4B1/PPPq1PP1/2KR4",
	"r1b3nr/pp3k1p/2pn1pp1/7q/3N4/3PR3/PPP3P1/R1B2K2",
	"2kr1r2/1pp2q2/p1nbb3/8/3P2pp/P1B1Pp2/1PP2P1P/1KNRQB1R",
	"8/p5pp/4p3/1P1k1p2/P1p5/2K3P1/8/8",
	"1k2r2r/2p5/ppQp2q1/5p1p/1P1P1R2/P1N4P/2P3P1/3R2K1",
	"4b3/p4p1k/5p1p/3pp3/8/2p1PQ2/P5PP/2q1BNK1",
	"2rq2k1/5p2/p3b2p/3Q2p1/1bP5/4R3/PP1N1PPP/6K1",
	"r4k2/pp2b1p1/3pPp2/2p4q/8/2N5/PP3QK1/R1B2R2",
	"3r2k1/p1Q2r1p/1p2pqpP/5p2/2B5/2P5/PP3PP1/R4K2",
	"1r1q1rk1/p1p1bppp/2pp1n2/8/3PPBb1/2NQ1N2/PPPR2PP/2K4R",
	"6k1/p4pp1/2p4p/4P3/1bp2B2/2N5/PqPQ1PPP/3R2K1",
	"r2qkbnr/ppp3pp/3p4/4n3/2B3b1/5N2/PPPP2PP/RNBQ1RK1",
	"5rk1/p1p4p/4p1p1/2pp1p2/3P4/2P1P1P1/P1P2P1P/1R3BK1",
	"8/R5pp/2p1p1k1/4P3/Np6/1P6/PKP1B2r/8",
	"8/p7/r7/6kp/5R2/8/4N1P1/1K6",
	"rnb1k2r/ppqpn1bp/5pp1/4p3/2P1P3/3P4/PPQ2PPP/R1B1KBNR",
	"r3r1k1/5ppp/p1pQ4/2p5/1qb5/5N2/P1P2PPP/3R1RK1",
	"r3k2r/pppq1p2/2npbbp1/4p2p/4P3/2PP2QN/PP2BPPP/RN3RK1",
	"3rr1k1/1p3pp1/p2p3p/4n3/5b2/4N1Q1/PR4PP/5RK1",
	"r1b2rk1/ppppqpp1/2n2n1p/6B1/N3P3/2Q2P2/PPP3PP/R3KBR1",
	"r4rk1/ppp1qpp1/2b1p2p/4P3/2P2n2/PB1Q1N2/5PPP/1R2R1K1",
	"r1bqk1nr/pp2bppp/2n1p3/1B1pP3/3P4/8/PP3PPP/RNBQK1NR",
	"3r1rk1/p4ppp/2RNb3/p7/8/P3PP2/5KPP/2R5",
	"R4bk1/7p/6p1/4r1B1/8/6P1/P4N2/7K",
	"6k1/1p1q2pp/2pp4/p1b1p3/2P4Q/P2P1bPP/1P1N1r2/2B4K",
	"5r2/p2k4/3prp2/2pNn2p/P3P1p1/2BP2PP/2P3K1/5R2",
	"r2qk2r/1b2bppp/p1n5/1ppp4/5B2/P1NP1N2/1PP2PPP/R2QR1K1",
	"r2qk2r/ppp2ppp/3pbn2/1Qb5/1P1n4/6P1/P3PPBP/RNB1K1NR",
	"r2qr1k1/p4ppp/2p2n2/3p4/Q2P4/2P1BP1P/P4P2/R3K2R",
	"8/6k1/4B1p1/3P2p1/q7/1p5P/4R3/1K6",
	"r1r1k3/ppN1p2p/5npB/3Qp3/8/2P5/Pq1N1PPP/R4RK1",
	"r4q1k/5Npp/p1Q1pn2/1p1p4/P2Pp3/1BP5/2P3PP/5RK1",
	"5k2/5pp1/1RN1pn1p/1p1p4/3P4/4PP1P/5KP1/1r6",
	"3b4/5p2/6p1/3pPk2/pB1P1P2/P4K2/1P6/8",
	"rnb1kb1r/ppp3pp/8/4Bp2/4P3/2N1nP2/PPP3PP/3RKBNR",
	"7r/pp2k1pp/3p2b1/8/4p1n1/NB5P/PP1K2P1/R4R2",
	"R4bk1/p1p2ppp/1p3q2/6n1/1P6/P1NB1N2/2P2PPP/5RK1",
	"1Q5r/3q1kp1/4p2p/pR1P1p2/5P2/B1P1P3/P5PP/6K1",
	"r3k2r/pb2bppp/1qp2n2/3pp1B1/4P3/2NB1P2/PPP3PP/R2Q1RK1",
	"2kr1b1Q/1bp1q1p1/p1npppp1/1p6/3PP3/8/PP3PPP/RNB1KB1R",
	"3r1b1k/pp1n3p/3p1n2/2p3Q1/8/2qP4/P1P3PP/R4R1K",
	"1k4rr/ppp5/3q1p1p/3n2p1/3R4/2P2BP1/PP2PP1P/4QRK1",
	"1qr2rk1/3b2pp/p1n5/1ppnppb1/8/PP1PPPNP/1B2BKP1/R1Q4R",
	"r1b5/p3k2p/1pnBP1r1/8/6p1/2P1R1P1/P4P1P/3R2K1",
	"r4rk1/pp1n3p/1qp2p2/3p2p1/3PnN1N/2P4P/PPQ2PP1/R4RK1",
	"3k4/1Q6/3b2b1/P1n1q2p/3p3P/8/3PB1r1/R3K2R",
	"2R2rk1/pp3p1p/6p1/1q3Q2/3P4/P3PP1P/5P2/R5K1",
	"rnbq1rk1/p5pp/2p2p2/1p3p2/3P4/1P2P1B1/5PPP/RN1Q1RK1",
	"8/5k2/R7/8/1p1P4/1P6/P1r4P/7K",
	"5rk1/2p2p2/p6Q/4p1p1/q3P3/bR4NP/2P2PP1/2K4R",
	"r1b1k2r/1pp2ppp/p4n2/3p4/4P3/2qB4/P1P2PPP/R1BQR1K1",
	"r3r1k1/1pp2ppp/p4b2/P3Nb2/3P1B2/2P5/5PPP/R3RK2",
	"rnb2rk1/2q1bppp/p2ppn2/1p6/3NP3/2P1BP2/PP1QN1PP/2KR1B1R",
	"r1bqr1k1/1pp2p1p/1p1p2nB/8/3pPP2/6Q1/PPP3PP/R4RK1",
	"r3k1r1/ppp4p/1b2p3/3pnp2/1P2p2q/P1P5/B2P1PPP/R1BQ1RK1",
	"r1br2k1/ppp1n1pp/5p2/8/2Bp2P1/N1P2P2/PP4P1/R2QK1N1",
	"r1bR2k1/5r1p/1pp1p1pB/pN2bp2/2P5/8/PP3PPP/1R4K1",
	"r1bqk1nr/pp1pppbp/2n3p1/8/3P1P2/2P2N2/PP4PP/RNBQKB1R",
	"r1bq1rk1/pp3ppp/3p1n2/4p3/1P2n3/P1Q3P1/3PPPBP/R1B1K1NR",
	"7r/2p3k1/5p2/6p1/6p1/1PP4P/P3qB2/4R1K1",
	"r1bq1rk1/2p2ppp/p1n1pn2/1p2P3/1b1P4/2NB1N2/PP1B1PPP/R2QK2R",
	"2b5/p5k1/1p1r1r2/1P2n1pp/2P2p2/4pP1P/4B1P1/R2R2NK",
	"3r1r2/p1b2pkp/1p1p1pn1/2pR4/2P5/1P2PN1P/P4PP1/1R2N1K1",
	"8/8/8/8/3Q4/2K5/k7/8",
	"q3k2r/1b1n1ppp/3bpn2/1p1p4/1PpP1P2/2P1P3/2BNN1PP/2BQ1RK1",
	"r1bk1b1r/ppp2ppp/2n1p3/8/2P5/P4P2/1P3PPP/RN2KB1R",
	"1r3rk1/pbp2ppp/6q1/2ppP3/3P3Q/1P3N2/P4PPP/R4RK1",
	"r5k1/2p1b1pp/p3p1p1/1p2P3/4b2n/4B1KP/PP3P2/R2R4",
	"1r2r2k/p4Qpp/5p2/2Bb4/3P2P1/2p1PK1P/2q2P2/7R",
	"2r4k/1b1q1pp1/1rN4p/1P1p3Q/P2Nn3/7P/5PP1/2R1R1K1",
	"r1bqk2r/pppp1ppp/2n5/2b1P3/2B3n1/5N2/PB1N1PPP/R2Q1RK1",
	"2kr1b1r/p4pp1/q4n1p/4p3/NnPpP1bB/RP3N2/2P1BPPP/Q4RK1",
	"r3k2r/pp1bBpp1/4p2p/qB1n4/8/5N2/PP3PPP/R2QK2R",
	"8/6k1/1K5p/6p1/6P1/7r/8/q7",
	"2k5/p7/1p6/5p1p/p6P/3r4/4r3/6K1",
	"rn1qr3/ppp2kpp/3bb3/8/4Q3/5N2/PPP2PPP/RNB1K2R",
	"6k1/p3nppp/1b1B4/3n4/6P1/4PP1q/P2P3P/2RR2K1",
	"5rk1/pp2ppbp/3p2p1/q4bPP/3P4/P1P2N2/2P1BP2/1K1R3R",
	"8/5kp1/8/pp2Pp2/2b2P1P/P1r2BP1/1R3K2/8",
	"2krr3/pp3p1p/2p3p1/2P2b2/2PP2n1/P2pP3/1B1K2PP/R4R2",
	"r2kr3/ppp3pp/3b1n2/3N2B1/2BR2b1/P7/1P3PPP/5RK1",
	"1r3rk1/p1p2pp1/2p1qb1p/4p3/2P1Q1P1/5N1P/PP3P2/2KR1B1R",
	"r1bqk2r/ppp2ppp/2np1n2/2b1p3/N3P3/5N2/PPPPBPPP/R1BQ1RK1",
	"1r1q1rk1/p3pp2/2pp1bpp/2p5/4P3/2NP1Q1P/PPP2PP1/1R3RK1",
	"8/3nk2p/2r2p1P/5Pp1/3K2P1/8/8/q7",
	"r1b2rk1/ppqp2pp/2n1p1n1/2b1PpB1/8/2P2NP1/PP2QPBP/RN2R1K1",
	"r4rk1/pp5p/3b2p1/3P4/7q/1Q6/PP3PBp/5R1K",
	"r2q1rk1/pp2n1b1/2p2np1/1P1p1p1p/3P3P/2NBPQB1/P4PP1/R4RK1",
	"2k5/8/8/4b1p1/5pPp/N4P2/6K1/8",
	"8/2p1R3/4Q2p/p6k/1p5p/5q2/PP4RP/7K",
	"1r4r1/2p1kp1p/pp1p1p1n/N2Bp3/P3P3/2PP3P/1P3PP1/R5K1",
	"2k3nr/ppp2ppp/4p3/2q5/7r/2N5/PPP1QPPP/3R1RK1",
	"8/6p1/5p2/8/2k3P1/1b3P1P/pQ6/B5K1",
	"5rk1/5pp1/p6p/4Pp2/8/P2qnQ1P/1Pr1NRPK/8",
	"r2qk2r/1pp3p1/p5p1/b2pn1P1/5B2/2P1P2P/PP2QP2/2KR3R",
	"r5k1/pppNnppp/1b1q2b1/1B1P4/2P3P1/7P/P4P2/R2Q1RK1",
	"2b5/p3rk1p/6p1/8/3R4/1P6/P5PP/6K1",
	"rnbqkb1r/pp1p2pp/2p5/3np1B1/7P/3P4/PPP1PPB1/RN1QK1NR",
	"8/p5pp/2R5/1p1K1k2/2pb4/2P5/3r3P/8",
	"2kr3r/p1pn4/1p5p/8/8/2P2P1B/P4P1P/R4RK1",
	"r2q1rk1/ppp2ppp/2n5/3p4/7b/4nP2/PPP1P1B1/RNB1K3",
	"r2qkbnr/pppb1ppp/2np4/1B2p3/4P3/1P3N2/P1PP1PPP/RNBQ1RK1",
	"8/p1p1k2p/2p3p1/1p3b2/1b6/1P2RP2/P1P3PP/6K1",
	"8/5R2/4rk2/4p1q1/7Q/1PP2P2/2P3PP/6K1",
	"2Nk3r/1p2npb1/p2p2pp/3Pp3/4P2P/4BP2/PP2KP2/R6R",
	"8/R1p1k3/p7/1p4n1/6P1/4P3/PP3P2/6K1",
	"2rq1rk1/pp1n1ppp/1np1p3/4P2b/1bPP4/1P2BN1P/P2NB1P1/R2Q1RK1",
	"8/pR4p1/4R3/5Qpk/3P4/2r2p2/P4P1b/3K4",
	"r1b1kb1r/pp3ppp/2n5/2Pn4/2B5/5N2/PPP2PPP/R1B1K2R",
	"r7/ppp1k1bp/4bnq1/4p1p1/2Q1P3/2P5/P2P1PPP/RNB2RK1",
	"r2k1b1r/1pp1nQpp/p1n5/8/4P3/1B1P4/P2q1PPP/3R1RK1",
	"5r2/1p1r2kp/p2Pn1p1/6p1/2P1pq2/Q2BNb2/PP1R3P/2K2R2",
	"r3k2r/pppq1pbp/2n1bnp1/3p4/1P2p3/P1P1P3/B2P1PPP/RNBQK1NR",
	"2kr3r/ppq1nppp/3bp3/3p4/3P2P1/2N1BN2/PPP2PK1/R2Q1R2",
	"5k2/1b4pp/2pq1p2/1pPp1P2/1P1P4/1r1B4/6PP/3R2K1",
	"r2qk1nr/pp1nppbp/2p3p1/3p4/3P1Bb1/2P1PN2/PP2BPPP/RN1QK2R",
	"2k1r3/2pp4/1p1b4/3P1qp1/1P1R1p1p/2PP1Q1P/3B1PP1/6K1",
	"6k1/pp3ppp/1q6/4P3/5Pb1/4b3/PP4PP/RN1Q3K",
	"b2r2k1/p3R1p1/2P2p1p/1P6/P7/7P/3p1PP1/3K4",
	"r1b1kb1r/p3pppp/1qpp1n2/8/2B1P3/2N5/PPP2PPP/R1BQ1RK1",
	"6k1/8/5Q2/5K2/8/8/8/8",
	"rn2k3/pbp5/1p1p3N/8/2Bbn3/5K2/PPP3PP/RNB4R",
	"1b4k1/p4rpp/1p6/2r1p3/2P1R3/6P1/P5KP/2R5",
	"1Q4k1/6pp/p1r1p3/1b1p1q2/3P1P2/1P5P/6P1/4R1K1",
	"r1b1Rbk1/pp1p1ppp/8/2pq4/5P2/5N2/PP1B2PP/R5K1",
	"r2q1rk1/pppnb1p1/2np3p/8/1PBPPp2/P1N4P/5PP1/R1BQR1K1",
	"r4rk1/2pq3p/p1p2p2/3n4/Q1pP1p1B/2P5/P4PPP/1R3RK1",
	"4r1k1/pp4p1/3q1pNp/3p1Q2/1b6/PP5P/1B3PP1/R4K2",
	"3Q4/4rk2/3p4/3bq3/2p1p1B1/P5P1/5P1P/4R1K1",
	"3B2k1/1p5r/p7/2P5/3Pp3/4Prp1/4R3/6K1",
	"r3kb1r/pp1n1ppp/2p1p3/6q1/3P4/3Q1N2/PPP2PPP/R3K2R",
	"6k1/5pp1/4p2p/3p4/Q1pN2Pn/2P1B2P/P3KP1q/1r6",
	"8/4k3/6pp/4Q3/8/2P5/5PPP/3R2K1",
	"2r3k1/1Q3pp1/3p1q2/p1p1n1p1/8/2P3PP/1P3PK1/3R4",
	"2rr2k1/p4ppp/1p2p3/4P3/3PB3/5P2/P7/R2R2K1",
	"3r2k1/1b1r1pp1/p1q1p2p/1pp5/4P3/PNPP2P1/2Q2P1P/4RB1K",
	"1r1r2k1/p1p2pp1/4b2p/4p3/PnP1P3/1P3NP1/5PBP/3R1RK1",
	"rn3k2/pp4p1/2p1p1qp/3p4/3P2P1/2N1P3/PPP1Q1K1/7R",
	"r6k/pp1q1prp/3bpp2/3p3Q/3P1P2/2N4R/PPP3PP/R5K1",
	"r7/pp6/6pb/7p/2k5/1prR3P/2P2PPK/2R5",
	"r1b1kbnr/pp2pppp/8/1B1p4/3P1B2/P3P3/1q1N1PPP/R2QK1NR",
	"rn1qkb1r/1pp1pp1p/3p1np1/p7/4PB2/3P1Q1P/PPP2PP1/RN2KB1R",
	"7r/8/Q2R3P/2p1p3/1k6/3P2N1/PPPN4/2K5",
	"8/P7/8/8/5k2/2P5/2P2PPP/2K5",
	"r1bq1rk1/ppp2ppp/3p1n2/n2B2B1/4P3/1QP2N2/P4PPP/R3K2R",
	"6r1/8/2k1p3/pp4P1/2p2P2/5KR1/PP6/8",
	"r2qkbnr/ppp1p1p1/2n1b3/3pP1Bp/3P1p2/2N2P2/PPP3PP/R2QKBNR",
	"1r4k1/r4npp/2N1p3/Pp2P3/2p2PP1/2P5/5K2/RR6",
	"8/8/p7/8/1p6/1k1K4/2n5/8",
	"rnbq1rk1/3n1pbp/p2p2p1/3Pp3/Pp2P1P1/4B1NP/1P3P2/RN1QKB1R",
	"8/5pp1/p1p1k2p/r1P5/P1R1K1P1/1P3P2/7P/8",
	"r2q4/pp1npQ1k/2p3pp/3n1b2/2Bb4/N7/PP4PP/R1B2N1K",
	"4r1k1/p1p2p2/7p/2NpP1pq/3N1P2/1PP3KP/P7/4R3",
	"2r2rk1/p3qpbp/1p4p1/4P3/8/5B2/P4PPP/1R1R2K1",
	"4r1k1/4q1pp/pb2P3/1pn3P1/2p1R3/6BK/PP1Q1P2/4R3",
	"8/1p4kp/3q1pp1/2R1p3/1P5P/PK6/5P2/8",
	"2k4r/1pp1Rpp1/p7/2n5/5rP1/8/8/3R2K1",
	"r1b5/2p3k1/p7/5p2/6pR/1P2Pp2/P1PP1K2/R7",
	"8/5kb1/4q2p/1p4pP/1P6/8/1PQ2PP1/6K1",
	"8/6pp/1pR5/8/3P4/4k3/PnB1r2P/5RK1",
	"3Q1rk1/3P2p1/2p1P1pp/p1P5/1p6/7P/1q4PK/4R3",
	"r2qkb1r/p2npp1p/3p1np1/2pP4/2B1P1b1/2N2N2/PP3PPP/R1BQK2R",
	"r7/p1kb3Q/2q1p1B1/2p1P3/1b1p4/P7/2P2PPP/R3R1K1",
	"2rqr1k1/pp1bb1pp/8/2pPn1P1/5P2/1PN5/P1PQB3/2KR3R",
	"r4b1r/ppp1k1pp/4q3/4p3/3n4/3P1Q2/PPP2PPP/RNB2RK1",
	"r3r1k1/p4pbp/b1p3p1/P1B1p3/2P1n3/1PN5/5PPP/RB1R2K1",
	"3R4/1BN2pkp/pP4p1/8/2b1n3/r5P1/4P2P/2R1K3",
	"r3r1k1/pb3ppp/1p3n2/5N2/1b6/2N1B3/PPP2P1P/2KR2R1",
	"rn3rk1/pb2q2p/1p4p1/3pNp2/2pPnP2/2P1P1Q1/PPBN3P/R3K2R",
	"r2qkb1r/ppp1p1p1/6pp/3pP3/3Pn1P1/4P2P/PPP5/RNBQK2R",
	"r1n2rk1/2p1qp1p/1p2b1p1/1P6/p2BnPP1/N1P2B2/PQ5P/3R1RK1",
	"2k4r/1b3pp1/p7/2B4p/1p2P3/5Pq1/PP2B3/R2Q1K2",
	"r4rk1/2qn1ppp/1p1bbn2/pP1p4/2pP4/2P2N2/PBBN1PPP/R2QR1K1",
	"2r3k1/2r2p1p/1b3p2/1P1Pp3/R7/3Q2Pq/3N1P1P/1R4K1",
	"rn2kb1r/pp3pp1/3p1q1p/1Bp2P2/8/8/PPP2PPP/RN1QK2R",
	"r5k1/4n1pp/1q1p1p2/1P1p4/3P4/1R1B1N2/1P1Q1PPP/6K1",
	"1R6/5pp1/1P2p1k1/4P3/3p1p2/7R/1r4PP/6K1",
	"8/8/1K1rPnpk/8/P2P3P/8/8/3R4",
	"8/2pk2p1/n4p2/4pP2/3pN2P/6P1/1r1K1P2/2R5",
	"r3r1k1/1pB2pp1/p4b1p/3R4/5P2/1P6/P1P3PP/6K1",
	"r1b2rk1/ppp2p1p/2q3p1/4N3/8/2P4P/PPP2PP1/R2Q1RK1",
	"2k3nr/ppp1qppp/2n5/8/4P3/2N5/PPP1bPP1/1RBr1NK1",
	"8/8/8/8/6k1/P5N1/1PP3PP/3R3K",
	"1r1qkb1r/2p2ppp/p1p1pn2/3p4/3P4/2N1P3/PPPB1PPP/R2Q1RK1",
	"r3k2r/pbqpbppp/1p2p3/2p5/2P1P3/P1P1B1P1/1P1NQP1P/3R1RK1",
	"8/RP6/5k2/2p5/2P5/4p1P1/2Kp1r1P/8",
	"1r3rk1/2pq1pp1/3p2np/Np1Pp3/1P2P3/2P1P2P/5QP1/R4RK1",
	"r1b1kbnr/pp3ppp/2np4/1q2p3/4P3/Q4N2/PPP2PPP/R1B1KB1R",
	"rnbqk2r/4bppp/p4n2/6N1/2Pp4/5Q2/PPP2PPP/RNB1R1K1",
	"r7/3k1ppp/5p2/5P2/6PP/1pBP1Rb1/1P2K3/8",
	"8/4kp2/2r4p/p3p3/1p1pP3/5P2/5K2/1R1B4",
	"r1bk3r/pppp1ppp/5b2/3PN3/2N5/B7/P4PPP/2R2RK1",
	"r1b1k2r/p3nppp/2pq4/3p4/8/8/PPP1BPPP/RN1QK2R",
	"3r2k1/p3b1p1/1p2qp2/7p/2P1p1N1/1P2B1QP/P4PP1/6K1",
	"rn1q1rk1/pp1bbppp/5n2/1Np5/3p4/1P1B1N2/P1PP1PPP/R1BQR1K1",
	"2k5/5pp1/1P5p/P7/5q2/3Q3P/5PPK/8",
	"r6r/pp1k1pp1/2pP3p/8/5n2/2N2P2/PPP4P/2KR3R",
	"8/7p/5p2/p2k2p1/8/1PK4P/6P1/8",
	"1Q6/p4k2/6pp/1P1P1p2/2P5/5P2/P4KPP/2q1rN2",
	"4r1k1/5pp1/p6p/5b1n/P1BR3B/8/2P3P1/2K5",
	"r4rk1/p5pp/1p6/n7/1R6/1P3BP1/P4P1P/3R2K1",
	"8/P3kpP1/N1b2n2/1N4K1/5P2/6P1/8/8",
	"rnb2rk1/1pq1bppp/p2ppn2/2p5/2P1P3/2NPBN1P/PP2BPP1/R2Q1RK1",
	"4rrk1/ppq2pp1/2pb3p/3pP3/4n2B/P2Q4/1PP2PPP/R3RNK1",
	"2r3k1/5pp1/2R1pn1p/1r1p4/3P4/3NP2P/P4PP1/R5K1",
	"r2Q1k1r/5p2/1p2npp1/p1p5/2P4p/P6P/1P3PP1/R3R1K1",
	"r1bq1r1k/bp4pn/p1pp3p/8/3PPp2/2N2N1P/PPQ2PP1/R1B1R1K1",
	"r2q1rk1/1p3ppp/p1bB4/n7/2BP2n1/P1P4N/2QN1PPP/R4RK1",
	"1r2k1r1/2p2p2/pp1bp2p/3q4/P1N1Q1pP/1P1P2P1/1BP2P2/R4RK1",
	"1k1r3r/1pp5/p3q3/1b1p1p1p/5Pp1/1Pb3P1/P1P2RBP/R2Q3K",
	"2kr3r/ppp4p/5N1p/8/5b2/2N4b/PPP2P1P/1K1R2R1",
	"1k1r2r1/p1q2p2/Pp5p/2P1p1p1/1P6/3n1N1P/4Q1PK/R7",
	"rnb1k2r/ppp2ppp/4pn2/8/1b5N/3P4/PPP1BPPP/R1BQ1RK1",
	"rn1k4/ppp3pp/3b4/3Pp3/1P6/2P2r1P/P2N1P2/R1B1K2R",
	"r2q1rk1/pp1nbppp/2p2n2/3p2B1/3P2b1/2NBPP2/PPQ1N1PP/R3K2R",
	"2kr3r/pp1n1p2/2pqpp2/7p/2BP4/2P2N1P/PP3PP1/3R1RK1",
	"r2qk2r/4bppp/p2pb3/3np3/1PPp4/3Q3P/P3BPPN/R4RK1",
	"r3k2r/1b1nbpp1/2qppn1p/8/3NPP2/P1N5/1PP2BPP/R2Q1RK1",
	"r2qk2r/pp3pp1/2nbbn1p/3p4/8/1BN2N2/PPPP1PPP/R1BQK2R",
	"2k1r3/ppp2ppp/2P2n2/8/1KP1r3/1PB3P1/P5BP/R6R",
	"5r2/6bk/2p3pp/p7/8/7K/2p1p3/8",
	"rn1k1q1r/ppbn3p/2p1p1p1/3p1P2/3P4/2NB4/PP2Q1PP/R1B2R1K",
	"r3kbnr/5ppp/pqn1p3/1p1pPb2/2pP1P2/2P2N2/PPB3PP/RNBQ1RK1",
	"3bk2r/2p2p2/NpP1p2p/p5p1/8/2P5/PP3PPP/3R2K1",
	"2k5/2p5/6p1/4p3/5q2/8/4K3/8",
	"rnbqkb1r/pppp1ppp/3Q4/4p3/8/8/PPP2PPP/RNB1KBNR",
	"r1bqk1nr/pp2bppp/2n5/3p4/3P4/2N3B1/PP2QPPP/R3KBNR",
	"1k4r1/pp6/4p1qn/2Q4p/8/2Pp2P1/PP5P/3R2RK",
	"q5r1/1p2k1p1/pb3n1p/3n1b2/1P1Pp3/B6N/P2QBPPP/R4RK1",
	"r2q1rk1/pb1n1pnp/1p1Pp1p1/2p1P3/2P3PP/3B1NN1/P4P2/R2QK2R",
	"rnbqkbnr/p1p1p1pp/1p1p1p2/8/3P4/2P1P3/PP3PPP/RNBQKBNR",
	"r3k2r/ppp1bp2/2Nq3p/3p2p1/3P4/2N1P1P1/PP3PP1/R2QK2R",
	"2rqk2r/Q4ppp/2p1pn2/3p4/3P1B2/P1PbP3/5PPP/3RK1NR",
	"r2qkb1r/pppbn3/3p1p1p/1B1Pp3/8/4BN2/PPP2PPP/R2QK2R",
	"1r6/4kp1P/5p2/p3pB2/Pp6/5R1K/8/8",
	"3rr1k1/ppp1qppp/2nbbn2/8/3p4/2PPPPPP/PP1QN1B1/RNB1K2R",
	"r2q2k1/p4ppp/2p5/2Npr3/6n1/1P2P2P/P1Q2PP1/2R2RK1",
	"r1bq1rk1/pp3pp1/2p2nnp/3p4/3PP3/1BN4P/PPP3PK/R1BQ1R2",
	"r3r1k1/pp2bp2/2p4p/2PpN1p1/3P1BPq/P7/4PPK1/R2Q1R2",
	"r4rk1/1p3q2/p2p4/2p5/P1Pb1PPp/1P5P/2QB3K/4RR2",
	"r3r1k1/1pp2p2/p7/7p/1B4q1/1BP5/PP3K1P/5R1R",
	"8/5R2/8/1k6/8/6P1/P4PKP/8",
	"r3k2r/2p2ppp/p1q1p3/1B2P3/3RPB2/P1P2P2/K1P1QP1P/7R",
	"r1b1kbnr/pppp1ppp/2n5/4P3/5P2/2N3P1/PqPBP2P/R2QKBNR",
	"2r3k1/Q4pp1/4p2p/3p4/Pr3P2/6KP/6P1/8",
	"r2qkb1r/ppp2ppp/2n1bn2/3pp3/P7/1PN1P3/1BPPNPPP/R2QKB1R",
	"r7/1pp2k2/5n2/p2p4/3P1Pb1/3B2R1/PP3B1P/R6K",
	"8/7p/8/4kp2/8/6PP/2K5/8",
	"rnb1r1k1/ppp2ppp/4q3/3pB3/3P3P/1NPB4/PP3PP1/R2Q1RK1",
	"6k1/4Bppp/p7/K1p3P1/1pb4P/2b5/8/8",
	"r7/pQqk4/2np1r2/6Np/8/8/1b1B2PP/4R2K",
	"r2q1rk1/1p1b2p1/2n4p/p3p1BP/2Pp4/PP1P1BN1/4QP2/R3R1K1",
	"8/6p1/1p6/5p1p/p2p1P1P/P2n1P2/2kBK3/8",
	"8/5kpp/4pp2/p7/3P1P1P/6r1/5KP1/8",
	"8/2r5/1k3p2/p5p1/8/P6R/K6P/8",
	"2k5/2p3p1/5p2/3Q4/8/b2P4/P1P5/1KB1r3",
	"r2qk2r/p4ppp/2pp4/1p4P1/3P4/N4P2/PPb4P/R1B1K1NR",
	"2r1r1k1/4Bpbp/p2p1Pp1/3P4/1pP5/1q1B4/3Q2PP/3R3K",
	"4k2r/6pp/B1N1p1b1/8/1p6/2P1b3/PP4PP/2KR4",
	"rnb2rk1/pp2nppp/2pp1q2/8/4P3/1NN1b3/PPP1QPPP/2KR1B1R",
	"8/8/6k1/2Q1p1p1/1PP1P1K1/6P1/8/8",
	"r2qk2r/1b2bpp1/p3p2p/Pp4N1/6Q1/3BB3/2P2PPP/R4RK1",
	"4R3/1p4k1/p4ppp/3B4/8/BP4P1/r4n1K/8",
	"8/p7/1p6/1Pk2p1p/P1p4P/2K1PP2/8/8",
	"3r1rk1/ppp1bppp/8/4P2n/2Bq1p2/1Q6/PP4PP/R1B2R1K",
	"4r1k1/3q1pp1/5n1p/pp1p4/2pP4/P1P1BP2/1PQ3PP/4R2K",
	"R3nb2/3k4/P1p1pP2/2PpP3/2r3p1/8/5K2/8",
	"2r3k1/5pP1/p3p2p/1p1pB3/3P4/5B1P/3Q1PPK/2q5",
	"3q1rk1/2pp1ppp/3b4/4P3/3rnQ2/8/1P3PPP/R1B2RK1",
	"2r2k2/p4ppp/8/1BP2p2/5Pn1/P5P1/8/4R2K",
	"8/3R4/3p1rnk/8/3np1P1/7P/7K/8",
	"1r3rk1/3b1pbp/pq1p2p1/2p2n2/2PP4/N4N2/PB1Q1PPP/R3R1K1",
	"rn1q1rk1/ppp1bppp/1n1p2b1/8/2PP2PP/2NB1P2/PP2N3/R1BQK2R"
};

void get_average_num_nodes(int depth) {
	Board board;
	init_board(&board);

	MoveList move_list;
	init_move_list(&move_list);

    uint64_t nodes_visited = 0;

	float average = 0;
	float average_total = 0;
	int tick = 500 / 50;

	printf("Evaluating Positions: ");
	for (int position = 0; position < 500; ++position) {
		if (position % tick == 0) {
			printf("=");
			fflush(stdout);
		}
		init_board(&board);
		populate_board_from_fen(&board, FEN_POSITIONS[position]);

		reset_move_list(&move_list);
		get_legal_moves(&board, &move_list, WHITE);
		average_total += move_list.count / 500.0f;

		nodes_visited = 0;
		get_best_move_id(&board, WHITE, depth, &nodes_visited);
		average += ((float)nodes_visited  * 0.001 / 500.0f);

		for (int i = 0; i < 64; ++i) {
			for (int j = 0; j < 64; ++j) {
				HISTORY_TABLE[i][j] = 0.0f;
			}
		}
	}
	printf("\n\n");

	printf("========================================================================\n");
	printf("==                            DEPTH TEST                              ==\n");
	printf("========================================================================\n");
	printf("Average number of kilo-nodes visited at depth %d: %f\n", depth, average);
	printf("Typical branching factor:                         %f\n", pow(1000.0f * average, 1.0 / depth));
	printf("Average number of legal moves:                    %f\n", average_total);
	printf("========================================================================\n");
}
