﻿int Engine::quiesCheck(Board &board, int beta,  int qdepth)  //1007, int check_depth)
{
    int val, incheck;
    int best, capture, bestmove; //hashflag,  //, kingidx;
    int futilityValue, futilityBase, old_alpha, piecedest;
    int evasionPrunable, moveCount;
    int ttdepth; //1021
//1004    bool givesCheck;
    
		// 统计寂静搜索的次数
#ifdef DEBUG
    nQuiescNodes++;
#endif
#ifdef QCHECK
//		QNodes++;

#endif

// sf191 not poll input in qsearch
/*
	if (board.m_timeout) //stop
        	return UNKNOWN_VALUE;
*/
//  board.m_nodes++;

////1006 checktime for main thread only
//if (board.thd_id==0)
//{ 	
//2019k sf10 not check timeout in qsearch
/*
// 6. 中断调用；
        if (board.m_timeout) //stop
        	return UNKNOWN_VALUE;
        board.m_nodes++;
      //m_time_check++;
        //if ((m_time_check &8191)==0)  //4095)==0)

        if ((board.m_nodes      & PollNodes)==0) //4095)==0) //8191)==0)  //4095)==0)
         {
            //if (board.IMaxTime <4000)
            if (board.thd_id==0)
          {
            BusyComm = BusyLine(UcciComm, false);
            if (BusyComm == UCCI_COMM_STOP)
            //if (BusyLine(UcciComm, false) == UCCI_COMM_STOP )
            {
            	// "stop"指令发送中止信号
            board.m_timeout= 1;	//stop
            //printBestmove(board.m_bestmove);
            printf("info quiesCheck stopped by GUI pv\n");
            fflush(stdout);
            	//return (board.m_side ? -INF-1 : INF+1);
            	return UNKNOWN_VALUE;
            	//longjmp(setjmpbuf,1);
            }

            else if (BusyComm == UCCI_COMM_QUIT)
            {
            	p_feedback_move = -1; //pass to Eychessu.cpp to quit the engine
            	//printf("info UCCI_COMM_QUIT p_feedback_move = %d\n", p_feedback_move);
			        //fflush(stdout);
            }
          }
         
            //if ((clock()-board.m_startime>=board.IMaxTime))  //m_depth>=14 &&
            //if ((GetTime()-board.m_startime>=board.IMaxTime))  //m_depth>=14 &&
            long long t_remain = board.IMaxTime - (GetTime()-board.m_startime);

    				if (t_remain <= 0)
            {


//    printf("info Search timout %d board.ply %d board.IMaxTime %d\n", (int)(GetTime() - board.m_startime), board.ply, board.IMaxTime);

            	    board.m_timeout= 1; //stop
            	    //printBestmove(board.m_bestmove);
            	    //printf("info quiesce timeout\n");
                  //fflush(stdout);
                    return UNKNOWN_VALUE;
                    //longjmp(setjmpbuf,1);
            }
            if (t_remain < 4000)    				
    					PollNodes = 1023;
//          
        }
//} //1006
*/  //2019k

incheck=board.m_hisrecord[board.m_hisindex-1].htab.Chk;
// 2. 和棋裁剪；
// Check for an immediate draw or maximum ply reached
//    if (   pos.is_draw(ss->ply)
//        || ss->ply >= MAX_PLY)
//        return (ss->ply >= MAX_PLY && !inCheck) ? evaluate(pos) : VALUE_DRAW;


    //if (pos.IsDraw())
    //if ((board.bitpiece & 0xfff00ffc) == 0) return 0;
    	// If this is too deep then just return the stand-pat score to avoid long
        // quiescence searches.
        if ( ((board.bitpiece & 0xfff00ffc) == 0) 
        		|| (qdepth <= -MAXQDEPTH) )  //test maxqdepth instead of MAXEXT or MAXPLY
        {
            return Evalscore(board);
            //1113 return (qdepth <= -MAXQDEPTH && !incheck) ? Evalscore(board) : 0;
        }
        	

    // 1. Return if there is a mate line bad enough
    if (board.ply-INF >= beta)
    	return (board.ply-INF);

    val=board.checkloop(1);
    if (val)
    	return val;

//incheck=board.m_hisrecord[board.m_hisindex-1].htab.Chk;
//draw pruning    // Check for an immediate draw or maximum ply reached
        // if draw score and no rook & 1N/C vs 1N/C 
//      if (ext_p_endgame==1 && (!incheck) && (abs(beta-1) <= ext_drawval))
//    	if ( bitCountMSB((board.bitpiece & 0xf0000000)) ==0 && bitCountMSB((board.bitpiece & 0x0ff00000)) <=2)
//    	if (board.ply < MAXEXT)	
//    	{ //printf("*** QS:draw beta=%d, bitpiece=%x\n", beta, board.bitpiece);
//         return (beta-1); // alpha
//      }
 
      
//if (incheck) printf("m_hisindex-1=%d, incheck=%d\n", m_hisindex-1, incheck);
//bool nonPV = (beta - alpha == 1);

#ifdef QHASH

 

    // Transposition table lookup, only when not in PV
    //if ( (qdepth >=check_depth) ) // && !incheck)   //sf191 88i
    //{
    	/*
    	if (PVNode)
        hashmove = ProbeMoveQ(board);
      else
    	{
    	 // Decide whether or not to include checks: this fixes also the type of
    // TT entry depth that we are going to use. Note that in qsearch we use
    // only two types of depth in TT: DEPTH_QS_CHECKS or DEPTH_QS_NO_CHECKS.
    ttDepth = inCheck || depth >= DEPTH_QS_CHECKS ? DEPTH_QS_CHECKS     ( 0)
                                                  : DEPTH_QS_NO_CHECKS; (-1)
    // Transposition table lookup
    posKey = pos.key();
    tte = TT.probe(posKey, ttHit);
    ttValue = ttHit ? value_from_tt(tte->value(), ss->ply) : VALUE_NONE;
    ttMove = ttHit ? tte->move() : MOVE_NONE;

    if (  !PvNode
        && ttHit
        && tte->depth() >= ttDepth        
        && ttValue != VALUE_NONE // Only in case of TT access race
        && (ttValue >= beta ? (tte->bound() & BOUND_LOWER)
                            : (tte->bound() & BOUND_UPPER)))
        return ttValue;
    	*/
    	  ttdepth = (incheck || qdepth >= 0) ? 0 : -1;   //1021
    	  int hashmove = 0;
    	  int hashdepth = 0;
        val = ProbeHashQ(beta, hashmove, board, hashdepth);  //0
        if (val != UNKNOWN_VALUE)
        {   //1020 debug probehashq 
        	 //   printf("probehashQ incheck=%d, val=%d\n", val);
        	 //1020   if ( hashdepth >= ( incheck || qdepth >= 0 ? 0 : -1) )  //1007
//1020 above means: hashdepth=0 hash are always satified (>= 0 or -1) and pruned
//        	    	 hashdepth=-1 hash if (incheck or depth=-1 
//but since eychessu hashdepth is aged (for hash replacement) by +=(p_movenum &31), 
//safe way is to pruned only for incheck or depth=0
//1021 bef    if ((qdepth >= 0)  || (qdepth < 0 && hashdepth ==1)   || incheck )
          if ( (hashdepth - 2) >= ttdepth  )  //1021  QS hashdepth can only be 0 or -1 encoded as 2 or 1
          { 
 //         	if (hashdepth > 2)  //other hits
 //         		hashhit++;	
 //         	else QShashhit++;
          		
        	  return val;
        	}       
 //       	    else QShashmiss++;
        }
      //}

    //}

#endif


    //int ncapsize;
    MoveTabStruct movetab[111]; //, ncapmovetab[64];
    MoveTabStruct tempmove;
    old_alpha = beta - 1; //alpha;
//    best = -INF;  //927  need init?
    
#ifdef DELTAN
    
    int opt_value;
    
    //if (!PVNode)
    opt_value = INF;  // for delta pruning
#endif
    moveCount = 0;
    
    if (incheck)
        best = futilityBase = -INF;   //927
    else	
    {   best = val = Evalscore(board);
        if (val >= beta)
        {  if (hashmove==0)  //
               RecordHash(HASH_BETA, 0, val, 0, board); //sf191 88e //2019k chg back from RecordHashQ
            return val; //beta;
        }        
        futilityBase = best + DELTA_MARGIN;        
    }
    


//1004 givesCheck=false;
bestmove=0;
int num=0;
//int gen_count=0;
for (int qphase=3; qphase--;)	// 2=hashmove, 1=capture, 0=genchk
{
    switch (qphase)
    {
    	case 2:	//hashmove
    		if (hashmove==0)
    			num=0;
    		else
    			num=1;
    		break;

    	case 1:	//capture
    		if (!incheck)
    		{
    				num = (board.m_side ? board.GenCapQS<1>(&movetab[0]) : board.GenCapQS<0>(&movetab[0]));
    					//1021 ifnot incheck and no capture and qdepth < check_depth, return QS to avoid long QS
    				//if (num==0 && qdepth < check_depth)  //1021
    				//	return best;	                 //1021	
    	  }
    	  else
    	  {
    	  	  num=board.GenChkEvasCap(&movetab[0], incheck);
/*
        //single-reply extension
        if (PVNode && board.ply < MAXEXT) //MAXPLY - 4) //32)
        {
        if (num <= 1)
//1007        	check_depth -= 2;
        else
        {
        	int gcount = 0;
        	for (int j=0; j<num; j++)
        	{
            //if (makemovechk(movetab[j].table) ==0)
            if (board.makemovenoeval(movetab[j].table, 1) >=0) //, 1) >=0)
            {
                board.unmakemovenoeval();
                gcount++;
                if (gcount > 1)
                    break;
            }
        	}
        	if (gcount <= 1)
        	{
            //single_ext=1;
//1007						check_depth -= 2;
        	}
       	}
      }
*/      
    	  }
    		break;
    		
// sf10 to search the QS moves. Because the depth is <= 0 here, only captures,
// queen promotions and checks (only if depth >= DEPTH_QS_CHECKS) will be generated.
      case 0:	//genchk
    		if (incheck==0)
    		{
        	if (qdepth >=check_depth)
            //&& alpha < beta
            { num=board.generate_checks(&movetab[0]); 
            }
        	else
            num=0;
        }
    		else
    		{
        		num=board.GenChkEvasNCap(&movetab[0], incheck);
    		}
    		break;
    }

    //Quicksort(0, num-1, movetab); //table, tabval);
    //Insertsort(num, movetab);

    for (int i=0;i<num;++i)
    {
      switch (qphase)
     {
    	 case 2:	//hashmove
      	tempmove.table.move = hashmove;
      	//if (tempmove.table.move==0
        //   ||
        if (board.LegalKiller(tempmove.table)==0
           )
             continue;
        break;
    	
       case 1:	//capture
      	tempmove.tabentry = GetNextMove(i, num, movetab);
      	if (tempmove.table.move==hashmove)
        		continue;
		    break;
		  
       case 0:	//genchk
      	tempmove.tabentry = GetNextMove(i, num, movetab);

      	if (tempmove.table.move==hashmove)
        		continue;
        break;
     } // end switch qphase
        moveCount++;
        piecedest = board.piece[tempmove.dest];	
        //1003
        
        //1004	givesCheck = board.gives_check(tempmove.from, tempmove.dest);
//927 futility pruning for QSPV ref sf10
        if ( !incheck 
        	  && (qphase!=0 && !(board.gives_check(tempmove.from, tempmove.dest)) )    //1004  !givesCheck
        	  && futilityBase > -WIN_VALUE)
        { 
        	//piecedest = board.piece[tempmove.dest];	
        	futilityValue = futilityBase + abs(pointtable[PIECE_IDX(piecedest)][nRank(tempmove.dest)][nFile(tempmove.dest)]);
        	if (futilityValue <= beta-1) //alpha)
          {
              best = std::max(best, futilityValue);
              continue;
          }
//1001      if (futilityBase <= beta-1) //alpha)     //sf10 && !pos.see_ge(move, VALUE_ZERO + 1))
          	if ((futilityBase <= beta-1) && !board.see_ge(tempmove.from, tempmove.dest,  1))
          { 
//1001      
#ifdef PRTBOARD
if (board.thd_id==0)
{	printf("\n***debug see_ge:");
	print_board(beta-1);
  com2char(charmove, tempmove.from, tempmove.dest );
  printf("  %s\n", charmove);
  fflush(stdout);
}
#endif	
/*    	 
          	 if (piecedest < B_HORSE1
        	      //|| (board.piece[tempmove.from]>=B_ROOK1
        	      || ((board.piece[tempmove.from]>>2)==ROOK  //exclude king capture for SEE()
        	      && piecedest < B_ROOK1))
             {
//        	      if (board.see(tempmove.from, tempmove.dest) < 0) //1 for chess promotion, castling, en passant
        				{ best = std::max(best, futilityBase);
        				  continue;
        				}
             }          	
*/          


             best = std::max(best, futilityBase);
             continue;
          
          } 
        } 

/*
      // Detect non-capture evasions that are candidates to be pruned
      evasionPrunable =    inCheck
                       &&  (depth != DEPTH_ZERO || moveCount > 2)
                       &&  bestValue > VALUE_MATED_IN_MAX_PLY
                       && !pos.capture(move);
                       
      // Don't search moves with negative SEE values
      if (  (!inCheck || evasionPrunable)
          && !pos.see_ge(move))
          continue;      
*/          
        // Detect non-capture evasions that are candidates to be pruned
		  if ((incheck)
			&& (qdepth != 0 || moveCount > 2) //>2
			&& best > -INF + 128   //VALUE_MATED_IN_MAX_PLY
			&& (piecedest == 0))        //!pos.capture(move);
			   evasionPrunable = 1;
			else
				 evasionPrunable=0;
				 
        // Don't search moves with negative SEE values
        //if (  (!incheck || evasionPrunable))
        if (  (!incheck || evasionPrunable) && !board.see_ge(tempmove.from, tempmove.dest,0))
      	{     continue;  //1002	
      	
          // && !pos.see_ge(move))
/*          
             if (piecedest < B_HORSE1
        	      //|| (board.piece[tempmove.from]>=B_ROOK1
        	      || ((board.piece[tempmove.from]>>2)==ROOK  //exclude king capture for SEE()
        	      && piecedest < B_ROOK1))
             {
        	      //int see_val =board.see(tempmove.from, tempmove.dest);
        	      //if (evasionPrunable)
        	      //	{
    //printf("***incheck=%d, qdepth=%d, moveCount=%d, best=%d, evasionPrunable=%d\n",incheck, qdepth, moveCount, best, evasionPrunable);   	      		
		//printf("***from=%d, dest=%d, piecefrom=%d, piecedest=%d, SEE=%d\n",tempmove.from, tempmove.dest, board.piece[tempmove.from], piecedest, see_val);
        				//}
        	      //if (see_val < 0)	
//        	      if (board.see(tempmove.from, tempmove.dest) < 0)
        	      { 	
        	      	continue;
        			  }
             } 
*/                                  
        }

        capture = board.makemove(tempmove.table,
        	 (incheck
						|| (kingattk_incl_horse[kingindex[board.boardsq[32+board.m_side]]][tempmove.dest]
						|| kingattk_incl_horse[kingindex[board.boardsq[32+board.m_side]]][tempmove.from]))
						);
				/*
				if (incheck
					|| (kingattk_incl_horse[kingindex[board.boardsq[32+board.m_side]]][tempmove.dest]
					|| kingattk_incl_horse[kingindex[board.boardsq[32+board.m_side]]][tempmove.from]))
        		capture = board.makemove(tempmove.table, 1);
        else
		        capture = board.makemove(tempmove.table, 0);
        */
				if (capture < 0)
				{ moveCount--; 	
        	continue;
        }
				//gen_count++;

        //if (capture >=B_KING)
        //	val = INF-ply;
        //else
        //val = -quiesCheck(-beta, -alpha, qdepth-1, check_depth);
        val = -quiesCheck(board,   1-beta, qdepth-1);  //1007, check_depth);
        board.unmakemove();

        if (board.m_timeout) //stop
        	return UNKNOWN_VALUE;
/* ref sf10 
// Check for a new best move
      if (value > bestValue)
      {
          bestValue = value;

          if (value > alpha)
          {
              bestMove = move;

              if (PvNode) // Update pv even in fail-high case
                  update_pv(ss->pv, move, (ss+1)->pv);

              if (PvNode && value < beta) // Update alpha here!
                  alpha = value;
              else
                  break; // Fail high
          }
       }
*/


        if (val >= beta)
        {

#ifdef QHASH

					//if ((	qdepth >=check_depth) && !incheck)   //1020
            RecordHashQ(HASH_BETA, ttdepth, val, tempmove.table.move, board);
            //1021 int recqdepth = (qdepth==0 ? 0 : 1-(p_movenum&31) );
            //1021 RecordHash(HASH_BETA, recqdepth, val, tempmove.table.move, board);
          //  else QShashrecmiss++;
#endif
            //printf("beta cutoff val=%d\n", val);
            return val; // beta;
        }
        if (val > best)
        {
            best = val;
            if (val > beta-1) //alpha)
            {
                //alpha = val;
				//beta = val + 1;
                bestmove = tempmove.table.move;
            }
        }

    }	// end for i (< num)

} // end for qphase

/* sf10  
// All legal moves have been searched. A special case: If we're in check
    // and no legal moves were found, it is checkmate.
    if (inCheck && bestValue == -VALUE_INFINITE)
        return mated_in(ss->ply); // Plies to mate from the root

    tte->save(posKey, value_to_tt(bestValue, ss->ply),
              bestValue >= beta ? BOUND_LOWER :
              PvNode && bestValue > oldAlpha  ? BOUND_EXACT : BOUND_UPPER,
              ttDepth, bestMove, ss->staticEval);

    assert(bestValue > -VALUE_INFINITE && bestValue < VALUE_INFINITE);

    return bestValue;
*/    

// 8. 处理
    if ((best == -INF))
//			if (gen_count==0 && incheck)	//mate!
			{
        //printf("ply-INF val=%d\n", board.ply-INF);
        return board.ply - INF;
    	}

#ifdef QHASH

    //if ((qdepth >=check_depth) && !incheck)
    //1020 if (qdepth >=check_depth)
    {
        //1021 int recqdepth = (qdepth==0 ? 0 : 1-(p_movenum&31) );
        //int hashflag; //, hashmv;
        //if (alpha == old_alpha)
     //1021 for QSPV only   if (best <= old_alpha) //sf191	//88e
        {
            RecordHashQ(HASH_ALPHA, ttdepth, best, bestmove, board);  //1021
            //1021 RecordHash(HASH_ALPHA, recqdepth, best, 0, board);
            //hashflag = HASH_ALPHA;
            //bestmove = 0;
        }
     //1021 for QSPV only   
     //1021   else
     //1021   {
            //hashflag = HASH_PV;
            //hashmove = ss->pv[ply];
     //1021       RecordHashQ(HASH_PV, ttdepth, best, bestmove, board);
            //1021 RecordHash(HASH_PV, recqdepth, best, bestmove, board);
     //1021   }

//RecHashQ:
        //RecordHashQ(hashflag, best, bestmove);
				//RecordHash(hashflag, 0, best, bestmove);

    }
    //else QShashrecmiss++;
#endif

    // 8. 处理
    //if (best == -INF) {
    //    return board.ply - INF;
    //}
    //printf("Q best val=%d\n", best);
    return best;
}
