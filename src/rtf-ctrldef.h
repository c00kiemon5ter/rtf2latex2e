/* The following defines are automatically generated from rtf-controls */
/* Do not edit. */

# define	rtfVersion	0

# define		rtfVersionNum	0

# define	rtfDefFont	1

# define		rtfDefFontNum	1

/*
 * Charset names
 */

# define	rtfCharSet	2

# define		rtfMacCharSet	2
# define		rtfAnsiCharSet	3
# define		rtfPcCharSet	4
# define		rtfPcaCharSet	5


/*
 * Destinations - minor numbers must be zero-based and sequential
 * rtfMinDestination and rtfMaxDestinations are set to the lowest
 * and highest destination numbers.  Note that the particular macros
 * to which they are equated may change if you add destinations!
 */

# define	rtfDestination	3

/* lowest destination number */
# define	rtfMinDestination	(rtfFontTbl)

# define		rtfFontTbl	6
# define		rtfFontAltName	7
# define		rtfEmbeddedFont	8
# define		rtfFontFile	9
# define		rtfFileTbl	10
# define		rtfFileInfo	11
# define		rtfColorTbl	12
# define		rtfStyleSheet	13
# define		rtfKeyCode	14
# define		rtfRevisionTbl	15
# define		rtfInfo	16
# define		rtfITitle	17
# define		rtfISubject	18
# define		rtfIAuthor	19
# define		rtfIOperator	20
# define		rtfIKeywords	21
# define		rtfIComment	22
# define		rtfIDoccomm	23
/* \verscomm may not exist -- was seen in earlier spec version */
# define		rtfIVerscomm	24
# define		rtfICreateTime	25
# define		rtfIRevisionTime	26
# define		rtfIPrintTime	27
# define		rtfIBackupTime	28
# define		rtfNextFile	29
# define		rtfTemplate	30
# define		rtfFNSep	31
# define		rtfFNContSep	32
# define		rtfFNContNotice	33
# define		rtfENSep	34
# define		rtfENContSep	35
# define		rtfENContNotice	36
# define		rtfParNumLevelStyle	37
# define		rtfHeader	38
# define		rtfFooter	39
# define		rtfHeaderLeft	40
# define		rtfHeaderRight	41
# define		rtfHeaderFirst	42
# define		rtfFooterLeft	43
# define		rtfFooterRight	44
# define		rtfFooterFirst	45
# define		rtfParNumText	46
# define		rtfParNumbering	47
# define		rtfParNumTextAfter	48
# define		rtfParNumTextBefore	49
# define		rtfBookmarkStart	50
# define		rtfBookmarkEnd	51
# define		rtfPict	52
# define		rtfObject	53
# define		rtfObjClass	54
# define		rtfObjName	55
# define		rtfObjTime	56
# define		rtfObjData	57
# define		rtfObjAlias	58
# define		rtfObjSection	59
# define		rtfObjResult	60
/* \objitem and \objtopic aren't documented in the spec! */
# define		rtfObjItem	61
# define		rtfObjTopic	62
# define		rtfDrawObject	63
# define		rtfFootnote	64
# define		rtfAnnotRefStart	65
# define		rtfAnnotRefEnd	66
# define		rtfAnnotID	67
# define		rtfAnnotAuthor	68
# define		rtfAnnotation	69
# define		rtfAnnotRef	70
# define		rtfAnnotTime	71
# define		rtfAnnotIcon	72
# define		rtfField	73
# define		rtfFieldInst	74
# define		rtfFieldResult	75
# define		rtfDataField	76
# define		rtfIndex	77
# define		rtfIndexText	78
# define		rtfIndexRange	79
# define		rtfTOC	80
/* NeXT-specific -- non-standard */
# define		rtfNeXTGraphic	81
/* NeXT-specific -- non-standard */
# define		rtfNeXTHelpMarker	82
/* NeXT-specific -- non-standard */
# define		rtfNeXTHelpLink	83
# define		rtfDrawTextBoxText	84
/* added by Ujwal Sathyam */
# define		rtfSp	85
# define		rtfWord97Object	86
# define		rtfWord97ObjectGroup	87
# define		rtfWord97Picture	88
# define		rtfWord97NoPicture	89
# define		rtfDocvar	90
# define		rtfUpr	91
# define		rtfFchars	92
# define		rtfLchars	93
# define		rtfPgdsctbl	94
# define		rtfTOCN	95

/* highest destination number */
# define	rtfMaxDestination	(rtfTOCN)

/* number of destinations */
# define	rtfNumDestinations	(rtfMaxDestination - rtfMinDestination + 1)

/*
 * Font families
 */

# define	rtfFontFamily	4

# define		rtfFFNil	96
# define		rtfFFRoman	97
# define		rtfFFSwiss	98
# define		rtfFFModern	99
# define		rtfFFScript	100
# define		rtfFFDecor	101
# define		rtfFFTech	102
# define		rtfFFBidirectional	103

/*
 * Font attributes
 */

# define	rtfFontAttr	5

# define		rtfFontCharSet	104
# define		rtfFontPitch	105
# define		rtfFontCodePage	106
# define		rtfFTypeNil	107
# define		rtfFTypeTrueType	108
# define		rtfAnsiCodePage	109

/*
 * Color names
 */

# define	rtfColorName	6

# define		rtfRed	110
# define		rtfGreen	111
# define		rtfBlue	112

/*
 * File table attributes
 */

# define	rtfFileAttr	7

# define		rtfFileNum	113
# define		rtfFileRelPath	114
# define		rtfFileOSNum	115

/*
 * File sources
 */

# define	rtfFileSource	8

# define		rtfSrcMacintosh	116
# define		rtfSrcDOS	117
# define		rtfSrcNTFS	118
# define		rtfSrcHPFS	119
# define		rtfSrcNetwork	120

/*
 * Style attributes
 */

# define	rtfStyleAttr	9

# define		rtfAdditive	121
# define		rtfBasedOn	122
# define		rtfNext	123

/*
 * Key code attributes
 */

# define	rtfKeyCodeAttr	10

# define		rtfAltKey	124
# define		rtfShiftKey	125
# define		rtfControlKey	126
# define		rtfFunctionKey	127

/*
 * Document formatting attributes
 */

# define	rtfDocAttr	11

# define		rtfDefTab	128
# define		rtfHyphHotZone	129
# define		rtfHyphConsecLines	130
# define		rtfHyphCaps	131
# define		rtfHyphAuto	132
# define		rtfLineStart	133
# define		rtfFracWidth	134
/*
 * \makeback was given in old version of spec, it's now
 * listed as \makebackup.  Accept both.
 */
# define		rtfMakeBackup	135
# define		rtfRTFDefault	136
# define		rtfPSOverlay	137
# define		rtfDocTemplate	138
# define		rtfDefLanguage	139

# define		rtfFENoteType	140
# define		rtfFNoteEndSect	141
# define		rtfFNoteEndDoc	142
# define		rtfFNoteText	143
# define		rtfFNoteBottom	144
# define		rtfENoteEndSect	145
# define		rtfENoteEndDoc	146
# define		rtfENoteText	147
# define		rtfENoteBottom	148
# define		rtfFNoteStart	149
# define		rtfENoteStart	150
# define		rtfFNoteRestartPage	151
# define		rtfFNoteRestart	152
# define		rtfFNoteRestartCont	153
# define		rtfENoteRestart	154
# define		rtfENoteRestartCont	155
# define		rtfFNoteNumArabic	156
# define		rtfFNoteNumLLetter	157
# define		rtfFNoteNumULetter	158
# define		rtfFNoteNumLRoman	159
# define		rtfFNoteNumURoman	160
# define		rtfFNoteNumChicago	161
# define		rtfENoteNumArabic	162
# define		rtfENoteNumLLetter	163
# define		rtfENoteNumULetter	164
# define		rtfENoteNumLRoman	165
# define		rtfENoteNumURoman	166
# define		rtfENoteNumChicago	167

# define		rtfPaperWidth	168
# define		rtfPaperHeight	169
# define		rtfPaperSize	170
# define		rtfLeftMargin	171
# define		rtfRightMargin	172
# define		rtfTopMargin	173
# define		rtfBottomMargin	174
# define		rtfFacingPage	175
# define		rtfGutterWid	176
# define		rtfMirrorMargin	177
# define		rtfLandscape	178
# define		rtfPageStart	179
# define		rtfWidowCtrl	180

# define		rtfLinkStyles	181

# define		rtfNoAutoTabIndent	182
# define		rtfWrapSpaces	183
# define		rtfPrintColorsBlack	184
# define		rtfNoExtraSpaceRL	185
# define		rtfNoColumnBalance	186
# define		rtfCvtMailMergeQuote	187
# define		rtfSuppressTopSpace	188
# define		rtfSuppressPreParSpace	189
# define		rtfCombineTblBorders	190
# define		rtfTranspMetafiles	191
# define		rtfSwapBorders	192
# define		rtfShowHardBreaks	193

# define		rtfFormProtected	194
# define		rtfAllProtected	195
# define		rtfFormShading	196
# define		rtfFormDisplay	197
# define		rtfPrintData	198

# define		rtfRevProtected	199
# define		rtfRevisions	200
# define		rtfRevDisplay	201
# define		rtfRevBar	202

# define		rtfAnnotProtected	203

# define		rtfRTLDoc	204
# define		rtfLTRDoc	205

/*
 * Section formatting attributes
 */

# define	rtfSectAttr	12

# define		rtfSectDef	206
# define		rtfENoteHere	207
# define		rtfPrtBinFirst	208
# define		rtfPrtBin	209
# define		rtfSectStyleNum	210
# define		rtfSectUnlocked	211

# define		rtfNoBreak	212
# define		rtfColBreak	213
# define		rtfPageBreak	214
# define		rtfEvenBreak	215
# define		rtfOddBreak	216

# define		rtfColumns	217
# define		rtfColumnSpace	218
# define		rtfColumnNumber	219
# define		rtfColumnSpRight	220
# define		rtfColumnWidth	221
# define		rtfColumnLine	222

# define		rtfLineModulus	223
# define		rtfLineDist	224
# define		rtfLineStarts	225
# define		rtfLineRestart	226
# define		rtfLineRestartPg	227
# define		rtfLineCont	228

# define		rtfSectPageWid	229
# define		rtfSectPageHt	230
# define		rtfSectMarginLeft	231
# define		rtfSectMarginRight	232
# define		rtfSectMarginTop	233
# define		rtfSectMarginBottom	234
# define		rtfSectMarginGutter	235
# define		rtfSectLandscape	236
# define		rtfTitleSpecial	237
# define		rtfHeaderY	238
# define		rtfFooterY	239

# define		rtfPageStarts	240
# define		rtfPageCont	241
# define		rtfPageRestart	242
# define		rtfPageNumRight	243
# define		rtfPageNumTop	244
# define		rtfPageDecimal	245
# define		rtfPageURoman	246
# define		rtfPageLRoman	247
# define		rtfPageULetter	248
# define		rtfPageLLetter	249
# define		rtfPageNumLevel	250
# define		rtfPageNumHyphSep	251
# define		rtfPageNumSpaceSep	252
# define		rtfPageNumColonSep	253
# define		rtfPageNumEmdashSep	254
# define		rtfPageNumEndashSep	255

/* \vertalt was misspelled as \vertal in specification 1.0 */
# define		rtfTopVAlign	256
# define		rtfBottomVAlign	257
# define		rtfCenterVAlign	258
# define		rtfJustVAlign	259

# define		rtfRTLSect	260
# define		rtfLTRSect	261

/*
 * I've seen these listed as section attributes in an old spec,
 * but not in real files...
 */
/* rtfNoBreak		nobreak*/
/* rtfColBreak		colbreak*/
/* rtfPageBreak		pagebreak*/
/* rtfEvenBreak		evenbreak*/
/* rtfOddBreak		oddbreak*/

/*
 * Paragraph formatting attributes
 */

# define	rtfParAttr	13

# define		rtfParDef	262
# define		rtfStyleNum	263
# define		rtfHyphenate	264
# define		rtfInTable	265
# define		rtfKeep	266
# define		rtfNoWidowControl	267
# define		rtfKeepNext	268
# define		rtfOutlineLevel	269
# define		rtfNoLineNum	270
# define		rtfPBBefore	271
# define		rtfSideBySide	272
# define		rtfQuadLeft	273
# define		rtfQuadRight	274
# define		rtfQuadJust	275
# define		rtfQuadCenter	276
# define		rtfFirstIndent	277
# define		rtfLeftIndent	278
# define		rtfRightIndent	279
# define		rtfSpaceBefore	280
# define		rtfSpaceAfter	281
# define		rtfSpaceBetween	282
# define		rtfSpaceMultiply	283

# define		rtfSubDocument	284

# define		rtfRTLPar	285
# define		rtfLTRPar	286

# define		rtfTabPos	287
/*
 * FrameMaker writes \tql (to mean left-justified tab, apparently)
 * although it's not in the spec.  It's also redundant, since lj
 * tabs are the default.
 */
# define		rtfTabLeft	288
# define		rtfTabRight	289
# define		rtfTabCenter	290
# define		rtfTabDecimal	291
# define		rtfTabBar	292
# define		rtfLeaderDot	293
# define		rtfLeaderHyphen	294
# define		rtfLeaderUnder	295
# define		rtfLeaderThick	296
# define		rtfLeaderEqual	297

# define		rtfParLevel	298
# define		rtfParBullet	299
# define		rtfParSimple	300
# define		rtfParNumCont	301
# define		rtfParNumOnce	302
# define		rtfParNumAcross	303
# define		rtfParHangIndent	304
# define		rtfParNumRestart	305
# define		rtfParNumCardinal	306
# define		rtfParNumDecimal	307
# define		rtfParNumULetter	308
# define		rtfParNumURoman	309
# define		rtfParNumLLetter	310
# define		rtfParNumLRoman	311
# define		rtfParNumOrdinal	312
# define		rtfParNumOrdinalText	313
# define		rtfParNumBold	314
# define		rtfParNumItalic	315
# define		rtfParNumAllCaps	316
# define		rtfParNumSmallCaps	317
# define		rtfParNumUnder	318
# define		rtfParNumDotUnder	319
# define		rtfParNumDbUnder	320
# define		rtfParNumNoUnder	321
# define		rtfParNumWordUnder	322
# define		rtfParNumStrikethru	323
# define		rtfParNumForeColor	324
# define		rtfParNumFont	325
# define		rtfParNumFontSize	326
# define		rtfParNumIndent	327
# define		rtfParNumSpacing	328
# define		rtfParNumInclPrev	329
# define		rtfParNumCenter	330
# define		rtfParNumLeft	331
# define		rtfParNumRight	332
# define		rtfParNumStartAt	333

# define		rtfBorderTop	334
# define		rtfBorderBottom	335
# define		rtfBorderLeft	336
# define		rtfBorderRight	337
# define		rtfBorderBetween	338
# define		rtfBorderBar	339
# define		rtfBorderBox	340
# define		rtfBorderSingle	341
# define		rtfBorderThick	342
# define		rtfBorderShadow	343
# define		rtfBorderDouble	344
# define		rtfBorderDot	345
# define		rtfBorderDash	346
# define		rtfBorderHair	347
# define		rtfBorderWidth	348
# define		rtfBorderColor	349
# define		rtfBorderSpace	350

# define		rtfShading	351
# define		rtfBgPatH	352
# define		rtfBgPatV	353
# define		rtfFwdDiagBgPat	354
# define		rtfBwdDiagBgPat	355
# define		rtfHatchBgPat	356
# define		rtfDiagHatchBgPat	357
# define		rtfDarkBgPatH	358
# define		rtfDarkBgPatV	359
# define		rtfFwdDarkBgPat	360
# define		rtfBwdDarkBgPat	361
# define		rtfDarkHatchBgPat	362
# define		rtfDarkDiagHatchBgPat	363
# define		rtfBgPatLineColor	364
# define		rtfBgPatColor	365

/*
 * Positioning attributes
 */

# define	rtfPosAttr	14

# define		rtfAbsWid	366
# define		rtfAbsHt	367

# define		rtfRPosMargH	368
# define		rtfRPosPageH	369
# define		rtfRPosColH	370
# define		rtfPosX	371
# define		rtfPosNegX	372
# define		rtfPosXCenter	373
# define		rtfPosXInside	374
# define		rtfPosXOutSide	375
# define		rtfPosXRight	376
# define		rtfPosXLeft	377

# define		rtfRPosMargV	378
# define		rtfRPosPageV	379
# define		rtfRPosParaV	380
# define		rtfPosY	381
# define		rtfPosNegY	382
# define		rtfPosYInline	383
# define		rtfPosYTop	384
# define		rtfPosYCenter	385
# define		rtfPosYBottom	386

# define		rtfNoWrap	387
# define		rtfDistFromTextAll	388
# define		rtfDistFromTextX	389
# define		rtfDistFromTextY	390
/*
 * \dyfrtext no longer exists as of spec 1.2, apparently
 * having been replaced by \dfrmtextx and \dfrmtexty.
 */
# define		rtfTextDistY	391

# define		rtfDropCapLines	392
# define		rtfDropCapType	393

/*
 * Table attributes
 */

# define	rtfTblAttr	15

# define		rtfRowDef	394
# define		rtfRowGapH	395
# define		rtfCellPos	396
# define		rtfMergeRngFirst	397
# define		rtfMergePrevious	398

# define		rtfRowLeft	399
# define		rtfRowRight	400
# define		rtfRowCenter	401
# define		rtfRowLeftEdge	402
# define		rtfRowHt	403
# define		rtfRowHeader	404
# define		rtfRowKeep	405

# define		rtfRTLRow	406
# define		rtfLTRRow	407

# define		rtfRowBordTop	408
# define		rtfRowBordLeft	409
# define		rtfRowBordBottom	410
# define		rtfRowBordRight	411
# define		rtfRowBordHoriz	412
# define		rtfRowBordVert	413

# define		rtfCellBordBottom	414
# define		rtfCellBordTop	415
# define		rtfCellBordLeft	416
# define		rtfCellBordRight	417

# define		rtfCellShading	418
# define		rtfCellBgPatH	419
# define		rtfCellBgPatV	420
# define		rtfCellFwdDiagBgPat	421
# define		rtfCellBwdDiagBgPat	422
# define		rtfCellHatchBgPat	423
# define		rtfCellDiagHatchBgPat	424
/*
 * The spec lists \clbgdkhor, but the corresponding non-cell
 * control is \bgdkhoriz.  At any rate, Macintosh Word seems
 * to accept both \clbgdkhor and \clbgdkhoriz.
 */
# define		rtfCellDarkBgPatH	425
# define		rtfCellDarkBgPatV	426
# define		rtfCellFwdDarkBgPat	427
# define		rtfCellBwdDarkBgPat	428
# define		rtfCellDarkHatchBgPat	429
# define		rtfCellDarkDiagHatchBgPat	430
# define		rtfCellBgPatLineColor	431
# define		rtfCellBgPatColor	432

/*
 * Character formatting attributes
 */

# define	rtfCharAttr	16

# define		rtfPlain	433
# define		rtfBold	434
# define		rtfAllCaps	435
# define		rtfDeleted	436
# define		rtfSubScript	437
# define		rtfSubScrShrink	438
# define		rtfNoSuperSub	439
# define		rtfExpand	440
# define		rtfExpandTwips	441
# define		rtfKerning	442
# define		rtfFontNum	443
# define		rtfFontSize	444
# define		rtfItalic	445
# define		rtfOutline	446
# define		rtfRevised	447
# define		rtfRevAuthor	448
# define		rtfRevDTTM	449
# define		rtfSmallCaps	450
# define		rtfShadow	451
# define		rtfStrikeThru	452
# define		rtfUnderline	453
# define		rtfDotUnderline	454
# define		rtfDbUnderline	455
# define		rtfNoUnderline	456
# define		rtfWordUnderline	457
# define		rtfSuperScript	458
# define		rtfSuperScrShrink	459
# define		rtfInvisible	460
# define		rtfForeColor	461
# define		rtfBackColor	462
# define		rtfRTLChar	463
# define		rtfLTRChar	464
# define		rtfCharStyleNum	465
# define		rtfCharCharSet	466
# define		rtfLanguage	467
/* this has disappeared from the spec as of 1.2 */
# define		rtfGray	468

/*
 * Associated character formatting attributes
 */

# define	rtfACharAttr	17

# define		rtfACBold	469
# define		rtfACAllCaps	470
# define		rtfACForeColor	471
# define		rtfACSubScript	472
# define		rtfACExpand	473
# define		rtfACFontNum	474
# define		rtfACFontSize	475
# define		rtfACItalic	476
# define		rtfACLanguage	477
# define		rtfACOutline	478
# define		rtfACSmallCaps	479
# define		rtfACShadow	480
# define		rtfACStrikeThru	481
# define		rtfACUnderline	482
# define		rtfACDotUnderline	483
# define		rtfACDbUnderline	484
# define		rtfACNoUnderline	485
# define		rtfACWordUnderline	486
# define		rtfACSuperScript	487

/*
 * Special characters
 */

# define	rtfSpecialChar	18

/* special characters seen in \info destination */

# define		rtfIIntVersion	488
# define		rtfIVersion	489
# define		rtfIEditTime	490
# define		rtfIYear	491
# define		rtfIMonth	492
# define		rtfIDay	493
# define		rtfIHour	494
# define		rtfIMinute	495
# define		rtfISecond	496
# define		rtfINPages	497
# define		rtfINWords	498
# define		rtfINChars	499
# define		rtfIIntID	500

/* other special characters */

# define		rtfCurHeadDate	501
# define		rtfCurHeadDateLong	502
# define		rtfCurHeadDateAbbrev	503
# define		rtfCurHeadTime	504
# define		rtfCurHeadPage	505
# define		rtfSectNum	506
# define		rtfCurFNote	507
# define		rtfCurAnnotRef	508
# define		rtfFNoteSep	509
# define		rtfFNoteCont	510
# define		rtfCell	511
# define		rtfRow	512
/*
 * newline and carriage return are synonyms for
 * \par when they are preceded by a \ character
 */
# define		rtfPar	513
# define		rtfSect	514
# define		rtfPage	515
# define		rtfColumn	516
# define		rtfLine	517
# define		rtfSoftPage	518
# define		rtfSoftColumn	519
# define		rtfSoftLine	520
# define		rtfSoftLineHt	521
# define		rtfTab	522
# define		rtfEmDash	523
# define		rtfEnDash	524
# define		rtfEmSpace	525
# define		rtfEnSpace	526
# define		rtfBullet	527
# define		rtfLQuote	528
# define		rtfRQuote	529
# define		rtfLDblQuote	530
# define		rtfRDblQuote	531
# define		rtfFormula	532
# define		rtfNoBrkSpace	533
# define		rtfNoReqHyphen	534
# define		rtfNoBrkHyphen	535
# define		rtfOptDest	536
# define		rtfLTRMark	537
# define		rtfRTLMark	538
# define		rtfNoWidthJoiner	539
# define		rtfNoWidthNonJoiner	540
/* is this a valid token? */
# define		rtfCurHeadPict	541

/*
 * Bookmark attributes
 */

# define	rtfBookmarkAttr	19

# define		rtfBookmarkFirstCol	542
# define		rtfBookmarkLastCol	543

/*
 * Picture attributes
 */

# define	rtfPictAttr	20

# define		rtfMacQD	544
# define		rtfPMMetafile	545
# define		rtfWinMetafile	546
# define		rtfDevIndBitmap	547
# define		rtfWinBitmap	548
# define		rtfPixelBits	549
# define		rtfBitmapPlanes	550
# define		rtfBitmapWid	551

# define		rtfPicWid	552
# define		rtfPicHt	553
/*
 * \picwGoal and \pichGoal aren't in the spec,
 * but some writers emit them, so we recognize them.
 */
# define		rtfPicGoalWid	554
# define		rtfPicGoalHt	555
# define		rtfPicScaleX	556
# define		rtfPicScaleY	557
# define		rtfPicScaled	558
# define		rtfPicCropTop	559
# define		rtfPicCropBottom	560
# define		rtfPicCropLeft	561
# define		rtfPicCropRight	562

# define		rtfPicMFHasBitmap	563
# define		rtfPicMFBitsPerPixel	564

# define		rtfPicBinary	565

/*
 * Object controls
 */

# define	rtfObjAttr	21

# define		rtfObjEmb	566
# define		rtfObjLink	567
# define		rtfObjAutoLink	568
# define		rtfObjSubscriber	569
# define		rtfObjPublisher	570
# define		rtfObjICEmb	571

# define		rtfObjLinkSelf	572
# define		rtfObjLock	573
# define		rtfObjUpdate	574

# define		rtfObjHt	575
# define		rtfObjWid	576
# define		rtfObjSetSize	577
# define		rtfObjAlign	578
# define		rtfObjTransposeY	579
# define		rtfObjCropTop	580
# define		rtfObjCropBottom	581
# define		rtfObjCropLeft	582
# define		rtfObjCropRight	583
# define		rtfObjScaleX	584
# define		rtfObjScaleY	585

# define		rtfObjResRTF	586
# define		rtfObjResPict	587
# define		rtfObjResBitmap	588
# define		rtfObjResText	589
# define		rtfObjResMerge	590

# define		rtfObjBookmarkPubObj	591
# define		rtfObjPubAutoUpdate	592

/*
 * Drawing object attributes
 */

# define	rtfDrawAttr	22

# define		rtfDrawLock	593
# define		rtfDrawPageRelX	594
# define		rtfDrawColumnRelX	595
# define		rtfDrawMarginRelX	596
# define		rtfDrawPageRelY	597
# define		rtfDrawParaRelY	598
# define		rtfDrawMarginRelY	599
# define		rtfDrawHeight	600

# define		rtfDrawBeginGroup	601
# define		rtfDrawGroupCount	602
# define		rtfDrawEndGroup	603
# define		rtfDrawArc	604
# define		rtfDrawCallout	605
# define		rtfDrawEllipse	606
# define		rtfDrawLine	607
# define		rtfDrawPolygon	608
# define		rtfDrawPolyLine	609
# define		rtfDrawRect	610
# define		rtfDrawTextBox	611

# define		rtfDrawOffsetX	612
# define		rtfDrawSizeX	613
# define		rtfDrawOffsetY	614
# define		rtfDrawSizeY	615

# define		rtfCOAngle	616
# define		rtfCOAccentBar	617
# define		rtfCOBestFit	618
# define		rtfCOBorder	619
# define		rtfCOAttachAbsDist	620
# define		rtfCOAttachBottom	621
# define		rtfCOAttachCenter	622
# define		rtfCOAttachTop	623
# define		rtfCOLength	624
# define		rtfCONegXQuadrant	625
# define		rtfCONegYQuadrant	626
# define		rtfCOOffset	627
# define		rtfCOAttachSmart	628
# define		rtfCODoubleLine	629
# define		rtfCORightAngle	630
# define		rtfCOSingleLine	631
# define		rtfCOTripleLine	632

# define		rtfDrawTextBoxMargin	633
# define		rtfDrawRoundRect	634

# define		rtfDrawPointX	635
# define		rtfDrawPointY	636
# define		rtfDrawPolyCount	637

# define		rtfDrawArcFlipX	638
# define		rtfDrawArcFlipY	639

# define		rtfDrawLineBlue	640
# define		rtfDrawLineGreen	641
# define		rtfDrawLineRed	642
# define		rtfDrawLinePalette	643
# define		rtfDrawLineDashDot	644
# define		rtfDrawLineDashDotDot	645
# define		rtfDrawLineDash	646
# define		rtfDrawLineDot	647
# define		rtfDrawLineGray	648
# define		rtfDrawLineHollow	649
# define		rtfDrawLineSolid	650
# define		rtfDrawLineWidth	651

# define		rtfDrawHollowEndArrow	652
# define		rtfDrawEndArrowLength	653
# define		rtfDrawSolidEndArrow	654
# define		rtfDrawEndArrowWidth	655
# define		rtfDrawHollowStartArrow	656
# define		rtfDrawStartArrowLength	657
# define		rtfDrawSolidStartArrow	658
# define		rtfDrawStartArrowWidth	659

# define		rtfDrawBgFillBlue	660
# define		rtfDrawBgFillGreen	661
# define		rtfDrawBgFillRed	662
# define		rtfDrawBgFillPalette	663
# define		rtfDrawBgFillGray	664
# define		rtfDrawFgFillBlue	665
# define		rtfDrawFgFillGreen	666
# define		rtfDrawFgFillRed	667
# define		rtfDrawFgFillPalette	668
# define		rtfDrawFgFillGray	669
# define		rtfDrawFillPatIndex	670

# define		rtfDrawShadow	671
# define		rtfDrawShadowXOffset	672
# define		rtfDrawShadowYOffset	673

/*
 * Footnote attributes
 */

# define	rtfFNoteAttr	23

# define		rtfFNAlt	674

/*
 * Field attributes
 */

# define	rtfFieldAttr	24

# define		rtfFieldDirty	675
# define		rtfFieldEdited	676
# define		rtfFieldLocked	677
# define		rtfFieldPrivate	678
# define		rtfFieldAlt	679

/*
 * Index entry attributes
 */

# define	rtfIndexAttr	25

# define		rtfIndexNumber	680
# define		rtfIndexBold	681
# define		rtfIndexItalic	682

/*
 * Table of contents attributes
 */

# define	rtfTOCAttr	26

# define		rtfTOCType	683
# define		rtfTOCLevel	684

/*
 * NeXT graphic attributes -- non-standard
 */

# define	rtfNeXTGrAttr	27

# define		rtfNeXTGWidth	685
# define		rtfNeXTGHeight	686

/*
 * Stuff added by Ujwal Sathyam for rtf2LaTeX2e
 * New paragraph attributes found in MS Word for Mac.
 * For some unknown reason, I couldn't add it to the list of the other
 * paragraph attributes. I have modified rtfprep.c to ignore multiply
 * defined majors. New destinations have to be added to the destination
 * list above.
 */

/*
 * More paragraph attributes
 */

# define		rtfWidowCtlPar	687
# define		rtfOutLineLevel	688

/*
 * More character attributes
 */

# define		rtfCGrid	689

/*
 * More section attributes
 */

# define		rtfAdjustRight	690

/*
 * More style attributes
 */

# define		rtfStyleAutoUpdate	691

/*
 * List attributes. List should actually be defined as a destination.
 */
 
# define	rtfListAttr	28
 
# define		rtfList	692
# define		rtfListTable	693
# define		rtfListOverrideTable	694
 
 /*
  * Word 97 Object Attributes
  */

# define	rtfWord97ObjAttr	29

# define		rtfWord97ObjResult	695
# define		rtfWord97ObjInst	696
# define		rtfWord97ObjText	697
# define		rtfShapeName	698
# define		rtfShapeValue	699

 /*
  * More table attributes
  */


# define		rtfVertTopAlign	700
# define		rtfVertCenterAlign	701
# define		rtfVertBottomAlign	702
# define		rtfVertLeftAlign	703
# define		rtfVertRightAlign	704
# define		rtfVertMergeRngFirst	705
# define		rtfVertMergeRngPrevious	706

 /*
  * More picture attributes
  */


# define		rtfPng	707
# define		rtfJpeg	708


/* some special ANSI encoding commands */

# define	rtfAnsiCharAttr	30

# define		rtfLowChar	709
# define		rtfHighChar	710
# define		rtfDblByteChar	711

