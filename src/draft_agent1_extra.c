#include "draft_signatures.h"

/* Unverified draft translated from the full original pseudocode */
/* TODO: Review arithmetic widths and decompiler paths before integration */
/* Native C locals replace original compiler spill slots */
/* TODO: External helper interfaces below remain provisional */
sint32 draft_agent1_missing_8003367C(sint32 x, sint32 y, sint32 coordinate);
void draft_agent1_missing_80035308(uint32 table, uint32 parameter, uint32 coordinates, uint32 output);

#define D1_U8(address) (*(uint8 *)psx_addr((uint32)(address), 1u))
#define D1_U16(address) (*(uint16 *)psx_addr((uint32)(address), 2u))
#define D1_S16(address) (*(sint16 *)psx_addr((uint32)(address), 2u))
#define D1_U32(address) (*(uint32 *)psx_addr((uint32)(address), 4u))

sint32 sub_8003132C(uint32 object, uint32 flags)
{
    sint32 v4;
    sint8 v5;
    sint32 v6;
    uint32 v7;
    uint32 v8;
    uint32 v9;
    sint32 v10;
    sint8 v11;
    uint32 v12;
    uint32 v13;
    sint32 v14;
    sint32 v15;
    uint32 v16;
    uint32 v17;
    sint32 v18;
    sint32 v19;
    uint32 v20;
    sint32 v21;
    uint32 v22;
    uint32 v23;
    uint32 v24;
    sint32 v25;
    sint32 v26;
    sint32 v27;
    sint32 v28;
    sint16 v29;
    sint16 v30;
    sint32 v31;
    sint32 v32;
    sint32 v33;
    sint32 v34;
    sint8 v35;
    sint32 v36;
    sint32 v37;
    sint32 v38;
    sint8 v39;
    sint32 v40;
    sint32 v41;
    sint32 v42;
    sint8 v43;
    sint32 v44;
    sint32 v45;
    sint32 v46;
    sint8 v47;
    sint32 v48;
    sint32 v49;
    sint32 v50;
    sint8 v51;
    sint32 v52;
    sint32 v53;
    sint32 v54;
    sint8 v55;
    sint32 v56;
    sint32 v57;
    sint8 v58;
    sint8 v59;
    sint8 v60;
    sint8 v61;
    sint8 v62;
    sint8 v63;
    sint8 v64;
    sint8 v65;
    sint32 v66;
    sint32 v67;
    sint32 v68;
    sint8 v69;
    sint32 v70;
    sint8 v71;
    sint32 v72;
    sint8 v73;
    sint32 v74;
    sint8 v75;
    sint32 v76;
    sint8 v77;
    sint32 v78;
    sint8 v79;
    sint32 v80;
    sint16 v81;
    sint16 v82;
    sint16 v83;
    sint8 v84;
    sint8 v85;
    sint8 v86;
    sint8 v87;
    sint8 v88;
    sint8 v89;
    sint32 v90;
    sint32 v91;
    uint32 v92;
    uint32 v93;
    sint32 v94;
    sint32 v95;
    sint32 v96;
    sint32 v97;
    sint16 v98;
    sint16 v99;
    sint32 v100;
    sint32 v101;
    sint32 v102;
    sint8 v103;
    sint32 v104;
    sint32 v105;
    sint32 v106;
    sint8 v107;
    sint32 v108;
    sint32 v109;
    sint32 v110;
    sint8 v111;
    sint32 v112;
    sint32 v113;
    sint32 v114;
    sint8 v115;
    sint32 v116;
    sint32 v117;
    sint32 v118;
    sint8 v119;
    sint32 v120;
    sint32 v121;
    sint32 v122;
    sint8 v123;
    sint32 v124;
    sint32 v125;
    sint32 v126;
    sint8 v127;
    sint32 v128;
    sint32 v129;
    sint32 v130;
    sint8 v131;
    sint32 v132;
    sint32 v133;
    sint8 v134;
    sint8 v135;
    sint8 v136;
    sint8 v137;
    sint8 v138;
    sint8 v139;
    sint8 v140;
    sint8 v141;
    sint8 v142;
    sint8 v143;
    sint8 v144;
    sint8 v145;
    sint32 v146;
    sint32 v147;
    sint32 v148;
    sint8 v149;
    sint32 v150;
    sint8 v151;
    sint32 v152;
    sint8 v153;
    sint32 v154;
    sint8 v155;
    sint32 v156;
    sint8 v157;
    sint32 v158;
    sint8 v159;
    sint32 v160;
    sint8 v161;
    sint32 v162;
    sint8 v163;
    sint32 v164;
    sint32 v165;
    sint32 v166;
    sint16 v167;
    sint16 v168;
    sint16 v169;
    sint8 v170;
    sint8 v171;
    sint8 v172;
    sint8 v173;
    sint8 v174;
    sint8 v175;
    sint8 v176;
    sint8 v177;
    uint32 v178;
    sint32 v179;
    uint32 v180;
    uint32 v181;
    uint32 v182;
    sint32 v183;
    sint32 v184;
    sint32 v185;
    sint32 v186;
    sint16 v187;
    sint16 v188;
    sint32 v189;
    sint32 v190;
    sint32 v191;
    sint8 v192;
    sint32 v193;
    sint32 v194;
    sint32 v195;
    sint8 v196;
    sint32 v197;
    sint32 v198;
    sint32 v199;
    sint8 v200;
    sint32 v201;
    sint32 v202;
    sint32 v203;
    sint8 v204;
    sint32 v205;
    sint32 v206;
    sint32 v207;
    sint8 v208;
    sint32 v209;
    sint32 v210;
    sint32 v211;
    sint8 v212;
    sint32 v213;
    sint32 v214;
    sint8 v215;
    sint8 v216;
    sint8 v217;
    sint8 v218;
    sint8 v219;
    sint8 v220;
    sint8 v221;
    sint8 v222;
    sint32 v223;
    sint32 v224;
    sint32 v225;
    sint8 v226;
    sint32 v227;
    sint8 v228;
    sint32 v229;
    sint8 v230;
    sint32 v231;
    sint8 v232;
    sint32 v233;
    sint8 v234;
    sint32 v235;
    sint8 v236;
    sint32 v237;
    sint16 v238;
    sint16 v239;
    sint16 v240;
    sint8 v241;
    sint8 v242;
    sint8 v243;
    sint8 v244;
    sint8 v245;
    sint8 v246;
    sint32 v247;
    uint32 v248;
    uint32 v249;
    sint32 v250;
    sint32 v251;
    sint32 v252;
    sint32 v253;
    sint16 v254;
    sint16 v255;
    sint32 v256;
    sint32 v257;
    sint32 v258;
    sint8 v259;
    sint32 v260;
    sint32 v261;
    sint32 v262;
    sint8 v263;
    sint32 v264;
    sint32 v265;
    sint32 v266;
    sint8 v267;
    sint32 v268;
    sint32 v269;
    sint32 v270;
    sint8 v271;
    sint32 v272;
    sint32 v273;
    sint32 v274;
    sint8 v275;
    sint32 v276;
    sint32 v277;
    sint32 v278;
    sint8 v279;
    sint32 v280;
    sint32 v281;
    sint32 v282;
    sint8 v283;
    sint32 v284;
    sint32 v285;
    sint32 v286;
    sint8 v287;
    sint32 v288;
    sint32 v289;
    sint8 v290;
    sint8 v291;
    sint8 v292;
    sint8 v293;
    sint8 v294;
    sint8 v295;
    sint8 v296;
    sint8 v297;
    sint8 v298;
    sint8 v299;
    sint8 v300;
    sint8 v301;
    sint32 v302;
    sint32 v303;
    sint32 v304;
    sint8 v305;
    sint32 v306;
    sint8 v307;
    sint32 v308;
    sint8 v309;
    sint32 v310;
    sint8 v311;
    sint32 v312;
    sint8 v313;
    sint32 v314;
    sint8 v315;
    sint32 v316;
    sint8 v317;
    sint32 v318;
    sint8 v319;
    sint32 v320;
    sint32 v321;
    sint32 v322;
    sint16 v323;
    sint16 v324;
    sint16 v325;
    sint8 v326;
    sint8 v327;
    sint8 v328;
    sint8 v329;
    sint8 v330;
    sint8 v331;
    sint8 v332;
    sint8 v333;
    sint32 result;
    uint32 v336;
    sint16 v337;
    sint32 v338;
    sint32 v339;
    sint16 v340;
    sint16 v341;
    sint16 v342;

    uint32 v377;
    uint32 v378;

    v339 = 0;
    v337 = D1_U16(0x8007A32Eu);
    v4 = ((D1_U8(object + 937) + D1_U32(0x80064C74u)) << 6) | (((sint32)D1_U32(0x80064C70u)) >> 4) & 0x3F;
    do
    {
        if (((flags >> v339) & 1) == 0)
            goto LABEL_455;
        if (v339)
        {
            if (v339 == 1)
            {
                v336 = D1_U32(object + 32);
            }
            else if (v339 == 2)
            {
                v336 = D1_U32(object + 36);
            }
            else
            {
                v336 = D1_U32(object + 28);
                if (((sint32)D1_U32(0x80064C14u)) >= D1_U8(object + 937) && D1_U32(object + 28) == D1_U32(object + 24))
                    goto LABEL_455;
            }
        }
        else
        {
            v336 = D1_U32(object + 24);
            if (((sint32)D1_U32(0x80064C14u)) < D1_U8(object + 937) && D1_U32(object + 24) == D1_U32(object + 28))
                goto LABEL_455;
        }
        if (v336)
        {
            v5 = 0;
            if (D1_S16(object + 74) > 0 && (!v339 || v339 == 3))
            {
                if (((sint32)D1_U32(0x80064C14u)) >= D1_U8(object + 937))
                {
                    if ((flags & 0x20) != 0)
                        v5 = 8;
                    goto LABEL_33;
                }
                if (v339 == 3)
                {
                    if ((flags & 0x20) != 0)
                        v5 = 32;
                    goto LABEL_33;
                }
                if (D1_U8(object + 82))
                {
                    v6 = 0;
                    if (((sint32)D1_U32(0x80064CB8u)) > 0)
                    {
                        v7 = 0x80073C3Cu;
                        do
                        {
                            if (D1_U32(v7) == v336)
                                break;
                            ++v6;
                            v7 += 4u;
                        } while (v6 < ((sint32)D1_U32(0x80064CB8u)));
                    }
                    v8 = 0x8007A3F8u + 4u * (uint32)v6;
                    v9 = D1_U32(v8);
                    if (D1_U32(v8))
                    {
                        if (v9 == object)
                            goto LABEL_33;
                        v340 = D1_U16(v9 + 74u);
                        v341 = D1_U16(v9 + 76u);
                        v5 = 64;
                        v342 = D1_U16(v9 + 78u);
                    }
                    else
                    {
                        v5 = 16;
                    }
                    D1_U32(v8) = object;
                }
            }
        LABEL_33:
            v10 = v339;
            if (!v339 && (flags & 0x10) != 0)
            {
                v5 |= 1u;
                v10 = 0;
            }
            if (v10 == 3 && (flags & 0x10) != 0)
                v5 |= 0x80u;
            v11 = v5 | 4;
            v12 = v336 + D1_U32(v336 + 44);
            if ((v11 & 1) != 0)
            {
                v13 = v336 + D1_U32(v336 + 44);

                if (((sint32)D1_U32(0x80064C14u)) < D1_U8(object + 937))
                {
                    draft_agent1_missing_80035308(0x8007A690u, D1_U32(0x800650B8u), v13, object + 338);
                }
                else
                {
                    draft_agent1_missing_80035308(0x8007A438u, D1_U32(0x800650B4u), v13, object + 338);
                }
            }
            v338 = 0;
            /* Cached UV origins are consumed only by the reuse branch */
            if ((v11 & 0x40) != 0)
                v14 = v341;
            while (1)
            {
                v15 = v338;
                v16 = (v336 + D1_U32(v336 + 48));
                v17 = v336 + D1_U32(4 * v338 + v336 + 52);
                if (D1_U8(v16 + 3u))
                    break;
            LABEL_454:
                v338 = v15 + 1;
                if (v15 + 1 >= 2)
                    goto LABEL_455;
            }
            while (1)
            {
                v18 = D1_U8(v16 + 2u);
                v19 = D1_S16(v16);
                v16 += 4u;
                if (v18 == 1)
                    break;
                if (v18 < 2)
                {
                    v20 = v17;
                    v21 = 0;
                    if (v19 > 0)
                    {
                        if ((v11 & 0x40) != 0)
                            v22 = v342;
                        v23 = v17 + 14;
                        while (2)
                        {
                            v24 = D1_U16(v23 + 8);
                            v25 = v11 & 0x40;
                            if (v24 < (uint16)D1_U16(0x8007A324u + 2u * (uint32)(0)))
                                goto LABEL_129;
                            v25 = v11 & 0x40;
                            if ((uint16)D1_U16(0x8007A32Cu) < v24)
                                goto LABEL_129;
                            v26 = v11 & 0xA0;
                            if ((v11 & 0x19) != 0)
                            {
                                v27 = (D1_U8(v23 - 1) + D1_U8(v23 + 7) + D1_U8(v23 + 15)) / 3;
                                v28 = (D1_U8(v23 - 2) + D1_U8(v23 + 6) + D1_U8(v23 + 14)) / 3;
                                if ((v11 & 1) != 0)
                                {
                                    D1_U16(v23 + 8) = D1_U16(0x8007A324u + 2u * (uint32)(D1_U8(object + draft_agent1_missing_8003367C(v28, v27, D1_S16(v12 + 8 * D1_S16(v16))) + 364)));
                                }
                                if ((v11 & 0x18) != 0 && v28 < 64)
                                {
                                    v26 = v11 & 0xA0;
                                    if ((uint32)(v27 - 33) < 0x3F)
                                    {
                                        v29 = D1_U16(object + 76);
                                        v30 = D1_U16(object + 78);
                                        v31 = 3 * D1_U8(v23 - 2);
                                        D1_U16(v23 + 8) = D1_U16(object + 74);
                                        v32 = v31 / 4;
                                        v33 = v31 / 4 < 0;
                                        v34 = v31 / 4 < 48;
                                        if (v33)
                                        {
                                            v35 = 0;
                                        }
                                        else
                                        {
                                            v35 = 47;
                                            if (v34)
                                                v35 = v32;
                                        }
                                        v36 = D1_U8(v23 + 6);
                                        D1_U8(v23 - 2) = v35 + v29;
                                        v37 = 3 * v36;
                                        v38 = v37 >> 2;
                                        if (v37 < 0)
                                            v38 = (v37 + 3) >> 2;
                                        if (v38 < 0)
                                        {
                                            v39 = 0;
                                        }
                                        else
                                        {
                                            v39 = 47;
                                            if (v38 < 48)
                                                v39 = v38;
                                        }
                                        v40 = D1_U8(v23 + 14);
                                        D1_U8(v23 + 6) = v39 + v29;
                                        v41 = 3 * v40;
                                        v42 = v41 >> 2;
                                        if (v41 < 0)
                                            v42 = (v41 + 3) >> 2;
                                        if (v42 < 0)
                                        {
                                            v43 = 0;
                                        }
                                        else
                                        {
                                            v43 = 47;
                                            if (v42 < 48)
                                                v43 = v42;
                                        }
                                        v44 = D1_U8(v23 - 1);
                                        D1_U8(v23 + 14) = v43 + v29;
                                        v45 = 3 * (v44 - 32);
                                        v46 = v45 >> 2;
                                        if (v45 < 0)
                                            v46 = (v45 + 3) >> 2;
                                        if (v46 < 0)
                                        {
                                            v47 = 0;
                                        }
                                        else
                                        {
                                            v47 = 47;
                                            if (v46 < 48)
                                                v47 = v46;
                                        }
                                        v48 = D1_U8(v23 + 7);
                                        D1_U8(v23 - 1) = v47 + v30;
                                        v49 = 3 * (v48 - 32);
                                        v50 = v49 >> 2;
                                        if (v49 < 0)
                                            v50 = (v49 + 3) >> 2;
                                        if (v50 < 0)
                                        {
                                            v51 = 0;
                                        }
                                        else
                                        {
                                            v51 = 47;
                                            if (v50 < 48)
                                                v51 = v50;
                                        }
                                        v52 = D1_U8(v23 + 15);
                                        D1_U8(v23 + 7) = v51 + v30;
                                        v53 = 3 * (v52 - 32);
                                        v54 = v53 >> 2;
                                        if (v53 < 0)
                                            v54 = (v53 + 3) >> 2;
                                        if (v54 < 0)
                                        {
                                            v55 = 0;
                                        }
                                        else
                                        {
                                            v55 = 47;
                                            if (v54 < 48)
                                                v55 = v54;
                                        }
                                        D1_U8(v23 + 15) = v55 + v30;
                                        goto LABEL_96;
                                    }
                                }
                                else
                                {
                                LABEL_96:
                                    v26 = v11 & 0xA0;
                                }
                            }
                            v33 = v26 == 0;
                            v25 = v11 & 0x40;
                            if (!v33)
                            {
                                v56 = ((sint8)(D1_U8(v23 - 1) + D1_U8(v23 + 7) + D1_U8(v23 + 15)) / 3) & 0x7F;
                                v57 = ((D1_U8(v23 - 2) + D1_U8(v23 + 6) + D1_U8(v23 + 14)) / 3) & 0x3F;
                                if ((v11 & 0x80) != 0)
                                {
                                    v58 = D1_U8(object + draft_agent1_missing_8003367C(2 * v57, 2 * v56, D1_S16(v12 + 8 * D1_S16(v16))) + 364);
                                    v59 = D1_U8(v23 + 6);
                                    v60 = D1_U8(v23 + 14);
                                    v61 = (v58 & 1) << 6;
                                    D1_U8(v23 - 2) = (D1_U8(v23 - 2) & 0x3F) + v61;
                                    v62 = D1_U8(v23 - 1);
                                    D1_U8(v23 + 6) = (v59 & 0x3F) + v61;
                                    v63 = D1_U8(v23 + 7);
                                    D1_U8(v23 + 14) = (v60 & 0x3F) + v61;
                                    v64 = (v58 & 2) << 6;
                                    v65 = (D1_U8(v23 + 15) & 0x7F) + v64;
                                    D1_U8(v23 - 1) = (v62 & 0x7F) + v64;
                                    D1_U8(v23 + 7) = (v63 & 0x7F) + v64;
                                    D1_U8(v23 + 15) = v65;
                                }
                                if ((v11 & 0x20) == 0 || v57 >= 32)
                                    goto LABEL_128;
                                v25 = v11 & 0x40;
                                if ((uint32)(v56 - 17) < 0x1F)
                                {
                                    v66 = D1_S16(object + 76);
                                    v67 = D1_S16(object + 78) - 24;
                                    v68 = ((3 * (D1_U8(v23 - 2) & 0x3F)) >> 1) + v66;
                                    D1_U16(v23 + 8) = D1_U16(object + 74);
                                    if (v68 < 0)
                                    {
                                        v69 = 0;
                                    }
                                    else
                                    {
                                        v69 = -1;
                                        if (v68 < 256)
                                            v69 = v68;
                                    }
                                    v70 = ((3 * (D1_U8(v23 + 6) & 0x3F)) >> 1) + v66;
                                    D1_U8(v23 - 2) = v69;
                                    if (v70 < 0)
                                    {
                                        v71 = 0;
                                    }
                                    else
                                    {
                                        v71 = -1;
                                        if (v70 < 256)
                                            v71 = v70;
                                    }
                                    v72 = ((3 * (D1_U8(v23 + 14) & 0x3F)) >> 1) + v66;
                                    D1_U8(v23 + 6) = v71;
                                    if (v72 < 0)
                                    {
                                        v73 = 0;
                                    }
                                    else
                                    {
                                        v73 = -1;
                                        if (v72 < 256)
                                            v73 = v72;
                                    }
                                    v74 = ((3 * (D1_U8(v23 - 1) & 0x7F)) >> 1) + v67;
                                    D1_U8(v23 + 14) = v73;
                                    if (v74 < 0)
                                    {
                                        v75 = 0;
                                    }
                                    else
                                    {
                                        v75 = -1;
                                        if (v74 < 256)
                                            v75 = v74;
                                    }
                                    v76 = ((3 * (D1_U8(v23 + 7) & 0x7F)) >> 1) + v67;
                                    D1_U8(v23 - 1) = v75;
                                    if (v76 < 0)
                                    {
                                        v77 = 0;
                                    }
                                    else
                                    {
                                        v77 = -1;
                                        if (v76 < 256)
                                            v77 = v76;
                                    }
                                    v78 = ((3 * (D1_U8(v23 + 15) & 0x7F)) >> 1) + v67;
                                    D1_U8(v23 + 7) = v77;
                                    if (v78 < 0)
                                    {
                                        v79 = 0;
                                    }
                                    else
                                    {
                                        v79 = -1;
                                        if (v78 < 256)
                                            v79 = v78;
                                    }
                                    D1_U8(v23 + 15) = v79;
                                LABEL_128:
                                    v25 = v11 & 0x40;
                                }
                            }
                        LABEL_129:
                            v33 = v25 == 0;
                            v80 = v11 & 4;
                            if (v33)
                                goto LABEL_135;
                            v80 = v11 & 4;
                            if (D1_U16(v23 + 8) != v340)
                                goto LABEL_135;
                            if ((uint32)((D1_U8(v23 - 2) + D1_U8(v23 + 6) + D1_U8(v23 + 14)) / 3 - v14 - 1) >= 0x2F)
                                goto LABEL_134;
                            v80 = v11 & 4;
                            if ((uint32)((D1_U8(v23 - 1) + D1_U8(v23 + 7) + D1_U8(v23 + 15)) / 3 - v22 - 1) < 0x2F)
                            {
                                v81 = D1_U16(object + 76);
                                v82 = D1_U16(object + 78);
                                v83 = D1_U16(object + 74);
                                v84 = D1_U8(v23 - 2);
                                v85 = D1_U8(v23 + 6);
                                v86 = D1_U8(v23 + 14);
                                v87 = D1_U8(v23 - 1);
                                v88 = D1_U8(v23 + 7);
                                v89 = D1_U8(v23 + 15);
                                D1_U16(v23) = v4;
                                D1_U16(v23 + 8) = v83;
                                D1_U8(v23 - 2) = v84 + v81 - v14;
                                D1_U8(v23 + 6) = v85 + v81 - v14;
                                D1_U8(v23 + 14) = v86 + v81 - v14;
                                D1_U8(v23 - 1) = v87 + v82 - v22;
                                D1_U8(v23 + 7) = v88 + v82 - v22;
                                D1_U8(v23 + 15) = v89 + v82 - v22;
                            LABEL_134:
                                v80 = v11 & 4;
                            }
                        LABEL_135:
                            if (v80)
                            {
                                if (D1_U16(v23 + 8) != v337)
                                    D1_U16(v23) = v4;
                            }
                            v23 += 32;
                            ++v21;
                            v20 += 32;
                            if (v21 >= v19)
                                break;
                            continue;
                        }
                    }
                    v17 = v20;
                    v90 = 20 * v19;
                    goto LABEL_451;
                }
                if (v18 == 2)
                {
                    v178 = v17;
                    v179 = 0;
                    if (v19 <= 0)
                    {
                    LABEL_339:
                        v17 = v178;
                        v90 = 44 * v19;
                        goto LABEL_451;
                    }
                    if ((v11 & 0x40) != 0)
                        v180 = v342;
                    v181 = v17 + 14;
                    while (2)
                    {
                        v182 = D1_U16(v181 + 12);
                        v183 = v11 & 0x40;
                        if (v182 >= (uint16)D1_U16(0x8007A324u + 2u * (uint32)(0)))
                        {
                            v183 = v11 & 0x40;
                            if ((uint16)D1_U16(0x8007A32Cu) >= v182)
                            {
                                v184 = v11 & 0xA0;
                                if ((v11 & 0x19) != 0)
                                {
                                    v185 = (D1_U8(v181 - 1) + D1_U8(v181 + 11) + D1_U8(v181 + 23)) / 3;
                                    v186 = (D1_U8(v181 - 2) + D1_U8(v181 + 10) + D1_U8(v181 + 22)) / 3;
                                    if ((v11 & 1) != 0)
                                    {
                                        D1_U16(v181 + 12) = D1_U16(0x8007A324u + 2u * (uint32)(D1_U8(object + draft_agent1_missing_8003367C(v186, v185, D1_S16(v12 + 8 * D1_S16(v16))) + 364)));
                                    }
                                    if ((v11 & 0x18) != 0 && v186 < 64)
                                    {
                                        v184 = v11 & 0xA0;
                                        if ((uint32)(v185 - 33) < 0x3F)
                                        {
                                            v187 = D1_U16(object + 76);
                                            v188 = D1_U16(object + 78);
                                            v189 = 3 * D1_U8(v181 - 2);
                                            D1_U16(v181 + 12) = D1_U16(object + 74);
                                            v190 = v189 / 4;
                                            v33 = v189 / 4 < 0;
                                            v191 = v189 / 4 < 48;
                                            if (v33)
                                            {
                                                v192 = 0;
                                            }
                                            else
                                            {
                                                v192 = 47;
                                                if (v191)
                                                    v192 = v190;
                                            }
                                            v193 = D1_U8(v181 + 10);
                                            D1_U8(v181 - 2) = v192 + v187;
                                            v194 = 3 * v193;
                                            v195 = v194 >> 2;
                                            if (v194 < 0)
                                                v195 = (v194 + 3) >> 2;
                                            if (v195 < 0)
                                            {
                                                v196 = 0;
                                            }
                                            else
                                            {
                                                v196 = 47;
                                                if (v195 < 48)
                                                    v196 = v195;
                                            }
                                            v197 = D1_U8(v181 + 22);
                                            D1_U8(v181 + 10) = v196 + v187;
                                            v198 = 3 * v197;
                                            v199 = v198 >> 2;
                                            if (v198 < 0)
                                                v199 = (v198 + 3) >> 2;
                                            if (v199 < 0)
                                            {
                                                v200 = 0;
                                            }
                                            else
                                            {
                                                v200 = 47;
                                                if (v199 < 48)
                                                    v200 = v199;
                                            }
                                            v201 = D1_U8(v181 - 1);
                                            D1_U8(v181 + 22) = v200 + v187;
                                            v202 = 3 * (v201 - 32);
                                            v203 = v202 >> 2;
                                            if (v202 < 0)
                                                v203 = (v202 + 3) >> 2;
                                            if (v203 < 0)
                                            {
                                                v204 = 0;
                                            }
                                            else
                                            {
                                                v204 = 47;
                                                if (v203 < 48)
                                                    v204 = v203;
                                            }
                                            v205 = D1_U8(v181 + 11);
                                            D1_U8(v181 - 1) = v204 + v188;
                                            v206 = 3 * (v205 - 32);
                                            v207 = v206 >> 2;
                                            if (v206 < 0)
                                                v207 = (v206 + 3) >> 2;
                                            if (v207 < 0)
                                            {
                                                v208 = 0;
                                            }
                                            else
                                            {
                                                v208 = 47;
                                                if (v207 < 48)
                                                    v208 = v207;
                                            }
                                            v209 = D1_U8(v181 + 23);
                                            D1_U8(v181 + 11) = v208 + v188;
                                            v210 = 3 * (v209 - 32);
                                            v211 = v210 >> 2;
                                            if (v210 < 0)
                                                v211 = (v210 + 3) >> 2;
                                            if (v211 < 0)
                                            {
                                                v212 = 0;
                                            }
                                            else
                                            {
                                                v212 = 47;
                                                if (v211 < 48)
                                                    v212 = v211;
                                            }
                                            D1_U8(v181 + 23) = v212 + v188;
                                            goto LABEL_296;
                                        }
                                    }
                                    else
                                    {
                                    LABEL_296:
                                        v184 = v11 & 0xA0;
                                    }
                                }
                                v33 = v184 == 0;
                                v183 = v11 & 0x40;
                                if (!v33)
                                {
                                    v213 = ((sint8)(D1_U8(v181 - 1) + D1_U8(v181 + 11) + D1_U8(v181 + 23)) / 3) & 0x7F;
                                    v214 = ((D1_U8(v181 - 2) + D1_U8(v181 + 10) + D1_U8(v181 + 22)) / 3) & 0x3F;
                                    if ((v11 & 0x80) != 0)
                                    {
                                        v215 = D1_U8(object + draft_agent1_missing_8003367C(2 * v214, 2 * v213, D1_S16(v12 + 8 * D1_S16(v16))) + 364);
                                        v216 = D1_U8(v181 + 10);
                                        v217 = D1_U8(v181 + 22);
                                        v218 = (v215 & 1) << 6;
                                        D1_U8(v181 - 2) = (D1_U8(v181 - 2) & 0x3F) + v218;
                                        v219 = D1_U8(v181 - 1);
                                        D1_U8(v181 + 10) = (v216 & 0x3F) + v218;
                                        v220 = D1_U8(v181 + 11);
                                        D1_U8(v181 + 22) = (v217 & 0x3F) + v218;
                                        v221 = (v215 & 2) << 6;
                                        v222 = (D1_U8(v181 + 23) & 0x7F) + v221;
                                        D1_U8(v181 - 1) = (v219 & 0x7F) + v221;
                                        D1_U8(v181 + 11) = (v220 & 0x7F) + v221;
                                        D1_U8(v181 + 23) = v222;
                                    }
                                    if ((v11 & 0x20) != 0 && v214 < 32)
                                    {
                                        v183 = v11 & 0x40;
                                        if ((uint32)(v213 - 17) < 0x1F)
                                        {
                                            v223 = D1_S16(object + 76);
                                            v224 = D1_S16(object + 78) - 24;
                                            v225 = ((3 * (D1_U8(v181 - 2) & 0x3F)) >> 1) + v223;
                                            D1_U16(v181 + 12) = D1_U16(object + 74);
                                            if (v225 < 0)
                                            {
                                                v226 = 0;
                                            }
                                            else
                                            {
                                                v226 = -1;
                                                if (v225 < 256)
                                                    v226 = v225;
                                            }
                                            v227 = ((3 * (D1_U8(v181 + 10) & 0x3F)) >> 1) + v223;
                                            D1_U8(v181 - 2) = v226;
                                            if (v227 < 0)
                                            {
                                                v228 = 0;
                                            }
                                            else
                                            {
                                                v228 = -1;
                                                if (v227 < 256)
                                                    v228 = v227;
                                            }
                                            v229 = ((3 * (D1_U8(v181 + 22) & 0x3F)) >> 1) + v223;
                                            D1_U8(v181 + 10) = v228;
                                            if (v229 < 0)
                                            {
                                                v230 = 0;
                                            }
                                            else
                                            {
                                                v230 = -1;
                                                if (v229 < 256)
                                                    v230 = v229;
                                            }
                                            v231 = ((3 * (D1_U8(v181 - 1) & 0x7F)) >> 1) + v224;
                                            D1_U8(v181 + 22) = v230;
                                            if (v231 < 0)
                                            {
                                                v232 = 0;
                                            }
                                            else
                                            {
                                                v232 = -1;
                                                if (v231 < 256)
                                                    v232 = v231;
                                            }
                                            v233 = ((3 * (D1_U8(v181 + 11) & 0x7F)) >> 1) + v224;
                                            D1_U8(v181 - 1) = v232;
                                            if (v233 < 0)
                                            {
                                                v234 = 0;
                                            }
                                            else
                                            {
                                                v234 = -1;
                                                if (v233 < 256)
                                                    v234 = v233;
                                            }
                                            v235 = ((3 * (D1_U8(v181 + 23) & 0x7F)) >> 1) + v224;
                                            D1_U8(v181 + 11) = v234;
                                            if (v235 < 0)
                                            {
                                                v236 = 0;
                                            }
                                            else
                                            {
                                                v236 = -1;
                                                if (v235 < 256)
                                                    v236 = v235;
                                            }
                                            D1_U8(v181 + 23) = v236;
                                            goto LABEL_328;
                                        }
                                    }
                                    else
                                    {
                                    LABEL_328:
                                        v183 = v11 & 0x40;
                                    }
                                }
                            }
                        }
                        v33 = v183 == 0;
                        v237 = v11 & 4;
                        if (!v33)
                        {
                            v237 = v11 & 4;
                            if (D1_U16(v181 + 12) == v340)
                            {
                                if ((uint32)((D1_U8(v181 - 2) + D1_U8(v181 + 10) + D1_U8(v181 + 22)) / 3 - v14 - 1) >= 0x2F)
                                    goto LABEL_334;
                                v237 = v11 & 4;
                                if ((uint32)((D1_U8(v181 - 1) + D1_U8(v181 + 11) + D1_U8(v181 + 23)) / 3 - v180 - 1) < 0x2F)
                                {
                                    v238 = D1_U16(object + 76);
                                    v239 = D1_U16(object + 78);
                                    v240 = D1_U16(object + 74);
                                    v241 = D1_U8(v181 - 2);
                                    v242 = D1_U8(v181 + 10);
                                    v243 = D1_U8(v181 + 22);
                                    v244 = D1_U8(v181 - 1);
                                    v245 = D1_U8(v181 + 11);
                                    v246 = D1_U8(v181 + 23);
                                    D1_U16(v181) = v4;
                                    D1_U16(v181 + 12) = v240;
                                    D1_U8(v181 - 2) = v241 + v238 - v14;
                                    D1_U8(v181 + 10) = v242 + v238 - v14;
                                    D1_U8(v181 + 22) = v243 + v238 - v14;
                                    D1_U8(v181 - 1) = v244 + v239 - v180;
                                    D1_U8(v181 + 11) = v245 + v239 - v180;
                                    D1_U8(v181 + 23) = v246 + v239 - v180;
                                LABEL_334:
                                    v237 = v11 & 4;
                                }
                            }
                        }
                        if (v237 && D1_U16(v181 + 12) != v337)
                            D1_U16(v181) = v4;
                        v181 += 40;
                        ++v179;
                        v178 += 40;
                        if (v179 >= v19)
                            goto LABEL_339;
                        continue;
                    }
                }
                if (v18 != 3)
                    goto LABEL_452;
                v378 = v17;
                v247 = 0;
                if (v19 > 0)
                {
                    v248 = v17 + 14;
                    while (2)
                    {
                        v249 = D1_U16(v248 + 12);
                        v250 = v11 & 0x40;
                        if (v249 < (uint16)D1_U16(0x8007A324u + 2u * (uint32)(0)))
                            goto LABEL_438;
                        v250 = v11 & 0x40;
                        if ((uint16)D1_U16(0x8007A32Cu) < v249)
                            goto LABEL_438;
                        v251 = v11 & 0xA0;
                        if ((v11 & 0x19) != 0)
                        {
                            v252 = (D1_U8(v248 - 2) + D1_U8(v248 + 10) + D1_U8(v248 + 22) + D1_U8(v248 + 34)) / 4;
                            v253 = (D1_U8(v248 - 1) + D1_U8(v248 + 11) + D1_U8(v248 + 23) + D1_U8(v248 + 35)) / 4;
                            if ((v11 & 1) != 0)
                            {
                                D1_U16(v248 + 12) = D1_U16(0x8007A324u + 2u * (uint32)(D1_U8(object + draft_agent1_missing_8003367C((D1_U8(v248 - 2) + D1_U8(v248 + 10) + D1_U8(v248 + 22) + D1_U8(v248 + 34)) / 4, v253, D1_S16(v12 + 8 * D1_S16(v16))) + 364)));
                            }
                            if ((v11 & 0x18) != 0 && v252 < 64)
                            {
                                v251 = v11 & 0xA0;
                                if ((uint32)(v253 - 33) < 0x3F)
                                {
                                    v254 = D1_U16(object + 76);
                                    v255 = D1_U16(object + 78);
                                    v256 = 3 * D1_U8(v248 - 2);
                                    D1_U16(v248 + 12) = D1_U16(object + 74);
                                    v257 = v256 / 4;
                                    v33 = v256 / 4 < 0;
                                    v258 = v256 / 4 < 48;
                                    if (v33)
                                    {
                                        v259 = 0;
                                    }
                                    else
                                    {
                                        v259 = 47;
                                        if (v258)
                                            v259 = v257;
                                    }
                                    v260 = D1_U8(v248 + 10);
                                    D1_U8(v248 - 2) = v259 + v254;
                                    v261 = 3 * v260;
                                    v262 = v261 >> 2;
                                    if (v261 < 0)
                                        v262 = (v261 + 3) >> 2;
                                    if (v262 < 0)
                                    {
                                        v263 = 0;
                                    }
                                    else
                                    {
                                        v263 = 47;
                                        if (v262 < 48)
                                            v263 = v262;
                                    }
                                    v264 = D1_U8(v248 + 22);
                                    D1_U8(v248 + 10) = v263 + v254;
                                    v265 = 3 * v264;
                                    v266 = v265 >> 2;
                                    if (v265 < 0)
                                        v266 = (v265 + 3) >> 2;
                                    if (v266 < 0)
                                    {
                                        v267 = 0;
                                    }
                                    else
                                    {
                                        v267 = 47;
                                        if (v266 < 48)
                                            v267 = v266;
                                    }
                                    v268 = D1_U8(v248 + 34);
                                    D1_U8(v248 + 22) = v267 + v254;
                                    v269 = 3 * v268;
                                    v270 = v269 >> 2;
                                    if (v269 < 0)
                                        v270 = (v269 + 3) >> 2;
                                    if (v270 < 0)
                                    {
                                        v271 = 0;
                                    }
                                    else
                                    {
                                        v271 = 47;
                                        if (v270 < 48)
                                            v271 = v270;
                                    }
                                    v272 = D1_U8(v248 - 1);
                                    D1_U8(v248 + 34) = v271 + v254;
                                    v273 = 3 * (v272 - 32);
                                    v274 = v273 >> 2;
                                    if (v273 < 0)
                                        v274 = (v273 + 3) >> 2;
                                    if (v274 < 0)
                                    {
                                        v275 = 0;
                                    }
                                    else
                                    {
                                        v275 = 47;
                                        if (v274 < 48)
                                            v275 = v274;
                                    }
                                    v276 = D1_U8(v248 + 11);
                                    D1_U8(v248 - 1) = v275 + v255;
                                    v277 = 3 * (v276 - 32);
                                    v278 = v277 >> 2;
                                    if (v277 < 0)
                                        v278 = (v277 + 3) >> 2;
                                    if (v278 < 0)
                                    {
                                        v279 = 0;
                                    }
                                    else
                                    {
                                        v279 = 47;
                                        if (v278 < 48)
                                            v279 = v278;
                                    }
                                    v280 = D1_U8(v248 + 23);
                                    D1_U8(v248 + 11) = v279 + v255;
                                    v281 = 3 * (v280 - 32);
                                    v282 = v281 >> 2;
                                    if (v281 < 0)
                                        v282 = (v281 + 3) >> 2;
                                    if (v282 < 0)
                                    {
                                        v283 = 0;
                                    }
                                    else
                                    {
                                        v283 = 47;
                                        if (v282 < 48)
                                            v283 = v282;
                                    }
                                    v284 = D1_U8(v248 + 35);
                                    D1_U8(v248 + 23) = v283 + v255;
                                    v285 = 3 * (v284 - 32);
                                    v286 = v285 >> 2;
                                    if (v285 < 0)
                                        v286 = (v285 + 3) >> 2;
                                    if (v286 < 0)
                                    {
                                        v287 = 0;
                                    }
                                    else
                                    {
                                        v287 = 47;
                                        if (v286 < 48)
                                            v287 = v286;
                                    }
                                    D1_U8(v248 + 35) = v287 + v255;
                                    goto LABEL_397;
                                }
                            }
                            else
                            {
                            LABEL_397:
                                v251 = v11 & 0xA0;
                            }
                        }
                        v33 = v251 == 0;
                        v250 = v11 & 0x40;
                        if (!v33)
                        {
                            v288 = ((sint8)(D1_U8(v248 - 1) + D1_U8(v248 + 11) + D1_U8(v248 + 23)) / 3) & 0x7F;
                            v289 = ((D1_U8(v248 - 2) + D1_U8(v248 + 10) + D1_U8(v248 + 22)) / 3) & 0x3F;
                            if ((v11 & 0x80) != 0)
                            {
                                v290 = D1_U8(object + draft_agent1_missing_8003367C(2 * v289, 2 * v288, D1_S16(v12 + 8 * D1_S16(v16))) + 364);
                                v291 = D1_U8(v248 + 34);
                                v292 = (v290 & 1) << 6;
                                v293 = (D1_U8(v248 + 10) & 0x3F) + v292;
                                v294 = D1_U8(v248 + 22) & 0x3F;
                                D1_U8(v248 - 2) = (D1_U8(v248 - 2) & 0x3F) + v292;
                                v295 = D1_U8(v248 - 1);
                                v296 = v294 + v292;
                                v297 = (v291 & 0x3F) + v292;
                                D1_U8(v248 + 10) = v293;
                                v298 = D1_U8(v248 + 11);
                                v299 = D1_U8(v248 + 23);
                                D1_U8(v248 + 22) = v296;
                                v300 = D1_U8(v248 + 35);
                                v301 = (v290 & 2) << 6;
                                D1_U8(v248 + 34) = v297;
                                D1_U8(v248 - 1) = (v295 & 0x7F) + v301;
                                D1_U8(v248 + 11) = (v298 & 0x7F) + v301;
                                D1_U8(v248 + 23) = (v299 & 0x7F) + v301;
                                D1_U8(v248 + 35) = (v300 & 0x7F) + v301;
                            }
                            if ((v11 & 0x20) == 0 || v289 >= 32)
                                goto LABEL_437;
                            v250 = v11 & 0x40;
                            if ((uint32)(v288 - 17) < 0x1F)
                            {
                                v302 = D1_S16(object + 76);
                                v303 = D1_S16(object + 78) - 24;
                                v304 = ((3 * (D1_U8(v248 - 2) & 0x3F)) >> 1) + v302;
                                D1_U16(v248 + 12) = D1_U16(object + 74);
                                if (v304 < 0)
                                {
                                    v305 = 0;
                                }
                                else
                                {
                                    v305 = -1;
                                    if (v304 < 256)
                                        v305 = v304;
                                }
                                v306 = ((3 * (D1_U8(v248 + 10) & 0x3F)) >> 1) + v302;
                                D1_U8(v248 - 2) = v305;
                                if (v306 < 0)
                                {
                                    v307 = 0;
                                }
                                else
                                {
                                    v307 = -1;
                                    if (v306 < 256)
                                        v307 = v306;
                                }
                                v308 = ((3 * (D1_U8(v248 + 22) & 0x3F)) >> 1) + v302;
                                D1_U8(v248 + 10) = v307;
                                if (v308 < 0)
                                {
                                    v309 = 0;
                                }
                                else
                                {
                                    v309 = -1;
                                    if (v308 < 256)
                                        v309 = v308;
                                }
                                v310 = ((3 * (D1_U8(v248 + 34) & 0x3F)) >> 1) + v302;
                                D1_U8(v248 + 22) = v309;
                                if (v310 < 0)
                                {
                                    v311 = 0;
                                }
                                else
                                {
                                    v311 = -1;
                                    if (v310 < 256)
                                        v311 = v310;
                                }
                                v312 = ((3 * (D1_U8(v248 - 1) & 0x7F)) >> 1) + v303;
                                D1_U8(v248 + 34) = v311;
                                if (v312 < 0)
                                {
                                    v313 = 0;
                                }
                                else
                                {
                                    v313 = -1;
                                    if (v312 < 256)
                                        v313 = v312;
                                }
                                v314 = ((3 * (D1_U8(v248 + 11) & 0x7F)) >> 1) + v303;
                                D1_U8(v248 - 1) = v313;
                                if (v314 < 0)
                                {
                                    v315 = 0;
                                }
                                else
                                {
                                    v315 = -1;
                                    if (v314 < 256)
                                        v315 = v314;
                                }
                                v316 = ((3 * (D1_U8(v248 + 23) & 0x7F)) >> 1) + v303;
                                D1_U8(v248 + 11) = v315;
                                if (v316 < 0)
                                {
                                    v317 = 0;
                                }
                                else
                                {
                                    v317 = -1;
                                    if (v316 < 256)
                                        v317 = v316;
                                }
                                v318 = ((3 * (D1_U8(v248 + 35) & 0x7F)) >> 1) + v303;
                                D1_U8(v248 + 23) = v317;
                                if (v318 < 0)
                                {
                                    v319 = 0;
                                }
                                else
                                {
                                    v319 = -1;
                                    if (v318 < 256)
                                        v319 = v318;
                                }
                                D1_U8(v248 + 35) = v319;
                            LABEL_437:
                                v250 = v11 & 0x40;
                            }
                        }
                    LABEL_438:
                        v33 = v250 == 0;
                        v320 = v11 & 4;
                        if (v33)
                            goto LABEL_446;
                        v320 = v11 & 4;
                        if (D1_U16(v248 + 12) != v340)
                            goto LABEL_446;
                        v321 = D1_U8(v248 - 2) + D1_U8(v248 + 10) + D1_U8(v248 + 22) + D1_U8(v248 + 34);
                        v322 = v321 >> 2;
                        if (v321 < 0)
                            v322 = (v321 + 3) >> 2;
                        if ((uint32)(v322 - v14 - 1) < 0x2F)
                        {
                            v320 = v11 & 4;
                            if ((uint32)((D1_U8(v248 - 1) + D1_U8(v248 + 11) + D1_U8(v248 + 23) + D1_U8(v248 + 35)) / 4 - v342 - 1) >= 0x2F)
                                goto LABEL_446;
                            v323 = D1_U16(object + 76);
                            v324 = D1_U16(object + 78);
                            v325 = D1_U16(object + 74);
                            v326 = D1_U8(v248 - 2);
                            v327 = D1_U8(v248 + 10);
                            v328 = D1_U8(v248 + 22);
                            v329 = D1_U8(v248 + 34);
                            v330 = D1_U8(v248 - 1);
                            v331 = D1_U8(v248 + 11);
                            v332 = D1_U8(v248 + 23);
                            v333 = D1_U8(v248 + 35);
                            D1_U16(v248) = v4;
                            D1_U16(v248 + 12) = v325;
                            D1_U8(v248 - 2) = v326 + v323 - v14;
                            D1_U8(v248 + 10) = v327 + v323 - v14;
                            D1_U8(v248 + 22) = v328 + v323 - v14;
                            D1_U8(v248 + 34) = v329 + v323 - v14;
                            D1_U8(v248 - 1) = v330 + v324 - v342;
                            D1_U8(v248 + 11) = v331 + v324 - v342;
                            D1_U8(v248 + 23) = v332 + v324 - v342;
                            D1_U8(v248 + 35) = v333 + v324 - v342;
                        }
                        v320 = v11 & 4;
                    LABEL_446:
                        if (v320 && D1_U16(v248 + 12) != v337)
                            D1_U16(v248) = v4;
                        v248 += 52;
                        ++v247;
                        v378 += 52;
                        if (v247 >= v19)
                            break;
                        continue;
                    }
                }
                v17 = v378;
                v90 = 56 * v19;
            LABEL_451:
                v16 += (uint32)v90;
            LABEL_452:
                if (!D1_U8(v16 + 3u))
                {
                    v15 = v338;
                    goto LABEL_454;
                }
            }
            v377 = v17;
            v91 = 0;
            if (v19 <= 0)
            {
            LABEL_250:
                v17 = v377;
                v90 = 20 * v19;
                goto LABEL_451;
            }
            v92 = v17 + 14;
            while (1)
            {
                v93 = D1_U16(v92 + 8);
                v94 = v11 & 0x40;
                if (v93 < (uint16)D1_U16(0x8007A324u + 2u * (uint32)(0)))
                    goto LABEL_238;
                v94 = v11 & 0x40;
                if ((uint16)D1_U16(0x8007A32Cu) < v93)
                    goto LABEL_238;
                v95 = v11 & 0xA0;
                if ((v11 & 0x19) != 0)
                {
                    v96 = (D1_U8(v92 - 2) + D1_U8(v92 + 6) + D1_U8(v92 + 14) + D1_U8(v92 + 22)) / 4;
                    v97 = (D1_U8(v92 - 1) + D1_U8(v92 + 7) + D1_U8(v92 + 15) + D1_U8(v92 + 23)) / 4;
                    if ((v11 & 1) != 0)
                    {
                        D1_U16(v92 + 8) = D1_U16(0x8007A324u + 2u * (uint32)(D1_U8(object + draft_agent1_missing_8003367C((D1_U8(v92 - 2) + D1_U8(v92 + 6) + D1_U8(v92 + 14) + D1_U8(v92 + 22)) / 4, v97, D1_S16(v12 + 8 * D1_S16(v16))) + 364)));
                    }
                    if ((v11 & 0x18) == 0 || v96 >= 64)
                        goto LABEL_197;
                    v95 = v11 & 0xA0;
                    if ((uint32)(v97 - 33) < 0x3F)
                        break;
                }
            LABEL_198:
                v33 = v95 == 0;
                v94 = v11 & 0x40;
                if (!v33)
                {
                    v132 = ((sint8)(D1_U8(v92 - 1) + D1_U8(v92 + 7) + D1_U8(v92 + 15)) / 3) & 0x7F;
                    v133 = ((D1_U8(v92 - 2) + D1_U8(v92 + 6) + D1_U8(v92 + 14)) / 3) & 0x3F;
                    if ((v11 & 0x80) != 0)
                    {
                        v134 = D1_U8(object + draft_agent1_missing_8003367C(2 * v133, 2 * v132, D1_S16(v12 + 8 * D1_S16(v16))) + 364);
                        v135 = D1_U8(v92 + 22);
                        v136 = (v134 & 1) << 6;
                        v137 = (D1_U8(v92 + 6) & 0x3F) + v136;
                        v138 = D1_U8(v92 + 14) & 0x3F;
                        D1_U8(v92 - 2) = (D1_U8(v92 - 2) & 0x3F) + v136;
                        v139 = D1_U8(v92 - 1);
                        v140 = v138 + v136;
                        v141 = (v135 & 0x3F) + v136;
                        D1_U8(v92 + 6) = v137;
                        v142 = D1_U8(v92 + 7);
                        v143 = D1_U8(v92 + 15);
                        D1_U8(v92 + 14) = v140;
                        v144 = D1_U8(v92 + 23);
                        v145 = (v134 & 2) << 6;
                        D1_U8(v92 + 22) = v141;
                        D1_U8(v92 - 1) = (v139 & 0x7F) + v145;
                        D1_U8(v92 + 7) = (v142 & 0x7F) + v145;
                        D1_U8(v92 + 15) = (v143 & 0x7F) + v145;
                        D1_U8(v92 + 23) = (v144 & 0x7F) + v145;
                    }
                    if ((v11 & 0x20) == 0 || v133 >= 32)
                        goto LABEL_237;
                    v94 = v11 & 0x40;
                    if ((uint32)(v132 - 17) < 0x1F)
                    {
                        v146 = D1_S16(object + 76);
                        v147 = D1_S16(object + 78) - 24;
                        v148 = ((3 * (D1_U8(v92 - 2) & 0x3F)) >> 1) + v146;
                        D1_U16(v92 + 8) = D1_U16(object + 74);
                        if (v148 < 0)
                        {
                            v149 = 0;
                        }
                        else
                        {
                            v149 = -1;
                            if (v148 < 256)
                                v149 = v148;
                        }
                        v150 = ((3 * (D1_U8(v92 + 6) & 0x3F)) >> 1) + v146;
                        D1_U8(v92 - 2) = v149;
                        if (v150 < 0)
                        {
                            v151 = 0;
                        }
                        else
                        {
                            v151 = -1;
                            if (v150 < 256)
                                v151 = v150;
                        }
                        v152 = ((3 * (D1_U8(v92 + 14) & 0x3F)) >> 1) + v146;
                        D1_U8(v92 + 6) = v151;
                        if (v152 < 0)
                        {
                            v153 = 0;
                        }
                        else
                        {
                            v153 = -1;
                            if (v152 < 256)
                                v153 = v152;
                        }
                        v154 = ((3 * (D1_U8(v92 + 22) & 0x3F)) >> 1) + v146;
                        D1_U8(v92 + 14) = v153;
                        if (v154 < 0)
                        {
                            v155 = 0;
                        }
                        else
                        {
                            v155 = -1;
                            if (v154 < 256)
                                v155 = v154;
                        }
                        v156 = ((3 * (D1_U8(v92 - 1) & 0x7F)) >> 1) + v147;
                        D1_U8(v92 + 22) = v155;
                        if (v156 < 0)
                        {
                            v157 = 0;
                        }
                        else
                        {
                            v157 = -1;
                            if (v156 < 256)
                                v157 = v156;
                        }
                        v158 = ((3 * (D1_U8(v92 + 7) & 0x7F)) >> 1) + v147;
                        D1_U8(v92 - 1) = v157;
                        if (v158 < 0)
                        {
                            v159 = 0;
                        }
                        else
                        {
                            v159 = -1;
                            if (v158 < 256)
                                v159 = v158;
                        }
                        v160 = ((3 * (D1_U8(v92 + 15) & 0x7F)) >> 1) + v147;
                        D1_U8(v92 + 7) = v159;
                        if (v160 < 0)
                        {
                            v161 = 0;
                        }
                        else
                        {
                            v161 = -1;
                            if (v160 < 256)
                                v161 = v160;
                        }
                        v162 = ((3 * (D1_U8(v92 + 23) & 0x7F)) >> 1) + v147;
                        D1_U8(v92 + 15) = v161;
                        if (v162 < 0)
                        {
                            v163 = 0;
                        }
                        else
                        {
                            v163 = -1;
                            if (v162 < 256)
                                v163 = v162;
                        }
                        D1_U8(v92 + 23) = v163;
                    LABEL_237:
                        v94 = v11 & 0x40;
                    }
                }
            LABEL_238:
                v33 = v94 == 0;
                v164 = v11 & 4;
                if (v33)
                    goto LABEL_246;
                v164 = v11 & 4;
                if (D1_U16(v92 + 8) != v340)
                    goto LABEL_246;
                v165 = D1_U8(v92 - 2) + D1_U8(v92 + 6) + D1_U8(v92 + 14) + D1_U8(v92 + 22);
                v166 = v165 >> 2;
                if (v165 < 0)
                    v166 = (v165 + 3) >> 2;
                if ((uint32)(v166 - v14 - 1) < 0x2F)
                {
                    v164 = v11 & 4;
                    if ((uint32)((D1_U8(v92 - 1) + D1_U8(v92 + 7) + D1_U8(v92 + 15) + D1_U8(v92 + 23)) / 4 - v342 - 1) >= 0x2F)
                        goto LABEL_246;
                    v167 = D1_U16(object + 76);
                    v168 = D1_U16(object + 78);
                    v169 = D1_U16(object + 74);
                    v170 = D1_U8(v92 - 2);
                    v171 = D1_U8(v92 + 6);
                    v172 = D1_U8(v92 + 14);
                    v173 = D1_U8(v92 + 22);
                    v174 = D1_U8(v92 - 1);
                    v175 = D1_U8(v92 + 7);
                    v176 = D1_U8(v92 + 15);
                    v177 = D1_U8(v92 + 23);
                    D1_U16(v92) = v4;
                    D1_U16(v92 + 8) = v169;
                    D1_U8(v92 - 2) = v170 + v167 - v14;
                    D1_U8(v92 + 6) = v171 + v167 - v14;
                    D1_U8(v92 + 14) = v172 + v167 - v14;
                    D1_U8(v92 + 22) = v173 + v167 - v14;
                    D1_U8(v92 - 1) = v174 + v168 - v342;
                    D1_U8(v92 + 7) = v175 + v168 - v342;
                    D1_U8(v92 + 15) = v176 + v168 - v342;
                    D1_U8(v92 + 23) = v177 + v168 - v342;
                }
                v164 = v11 & 4;
            LABEL_246:
                if (v164 && D1_U16(v92 + 8) != v337)
                    D1_U16(v92) = v4;
                v92 += 40;
                ++v91;
                v377 += 40;
                if (v91 >= v19)
                    goto LABEL_250;
            }
            v98 = D1_U16(object + 76);
            v99 = D1_U16(object + 78);
            v100 = 3 * D1_U8(v92 - 2);
            D1_U16(v92 + 8) = D1_U16(object + 74);
            v101 = v100 / 4;
            v33 = v100 / 4 < 0;
            v102 = v100 / 4 < 48;
            if (v33)
            {
                v103 = 0;
            }
            else
            {
                v103 = 47;
                if (v102)
                    v103 = v101;
            }
            v104 = D1_U8(v92 + 6);
            D1_U8(v92 - 2) = v103 + v98;
            v105 = 3 * v104;
            v106 = v105 >> 2;
            if (v105 < 0)
                v106 = (v105 + 3) >> 2;
            if (v106 < 0)
            {
                v107 = 0;
            }
            else
            {
                v107 = 47;
                if (v106 < 48)
                    v107 = v106;
            }
            v108 = D1_U8(v92 + 14);
            D1_U8(v92 + 6) = v107 + v98;
            v109 = 3 * v108;
            v110 = v109 >> 2;
            if (v109 < 0)
                v110 = (v109 + 3) >> 2;
            if (v110 < 0)
            {
                v111 = 0;
            }
            else
            {
                v111 = 47;
                if (v110 < 48)
                    v111 = v110;
            }
            v112 = D1_U8(v92 + 22);
            D1_U8(v92 + 14) = v111 + v98;
            v113 = 3 * v112;
            v114 = v113 >> 2;
            if (v113 < 0)
                v114 = (v113 + 3) >> 2;
            if (v114 < 0)
            {
                v115 = 0;
            }
            else
            {
                v115 = 47;
                if (v114 < 48)
                    v115 = v114;
            }
            v116 = D1_U8(v92 - 1);
            D1_U8(v92 + 22) = v115 + v98;
            v117 = 3 * (v116 - 32);
            v118 = v117 >> 2;
            if (v117 < 0)
                v118 = (v117 + 3) >> 2;
            if (v118 < 0)
            {
                v119 = 0;
            }
            else
            {
                v119 = 47;
                if (v118 < 48)
                    v119 = v118;
            }
            v120 = D1_U8(v92 + 7);
            D1_U8(v92 - 1) = v119 + v99;
            v121 = 3 * (v120 - 32);
            v122 = v121 >> 2;
            if (v121 < 0)
                v122 = (v121 + 3) >> 2;
            if (v122 < 0)
            {
                v123 = 0;
            }
            else
            {
                v123 = 47;
                if (v122 < 48)
                    v123 = v122;
            }
            v124 = D1_U8(v92 + 15);
            D1_U8(v92 + 7) = v123 + v99;
            v125 = 3 * (v124 - 32);
            v126 = v125 >> 2;
            if (v125 < 0)
                v126 = (v125 + 3) >> 2;
            if (v126 < 0)
            {
                v127 = 0;
            }
            else
            {
                v127 = 47;
                if (v126 < 48)
                    v127 = v126;
            }
            v128 = D1_U8(v92 + 23);
            D1_U8(v92 + 15) = v127 + v99;
            v129 = 3 * (v128 - 32);
            v130 = v129 >> 2;
            if (v129 < 0)
                v130 = (v129 + 3) >> 2;
            if (v130 < 0)
            {
                v131 = 0;
            }
            else
            {
                v131 = 47;
                if (v130 < 48)
                    v131 = v130;
            }
            D1_U8(v92 + 23) = v131 + v99;
        LABEL_197:
            v95 = v11 & 0xA0;
            goto LABEL_198;
        }
    LABEL_455:
        result = ++v339 < 4;
    } while (v339 < 4);
    return result;
}
