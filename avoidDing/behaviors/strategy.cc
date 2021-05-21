#include "naobehavior.h"
#include "../rvdraw/rvdraw.h"

extern int agentBodyType;

/*
 * Real game beaming.
 * Filling params x y angle
 */
void NaoBehavior::beam( double& beamX, double& beamY, double& beamAngle ) {
    beamX = -HALF_FIELD_X + worldModel->getUNum();
    beamY = 0;
    beamAngle = 0;
}


SkillType NaoBehavior::selectSkill() {
    // My position and angle
    //cout << worldModel->getUNum() << ": " << worldModel->getMyPosition() << ",\t" << worldModel->getMyAngDeg() << "\n";

    // Position of the ball
    //cout << worldModel->getBall() << "\n";

    // Example usage of the roboviz drawing system and RVSender in rvdraw.cc.
    // Agents draw the position of where they think the ball is
    // Also see example in naobahevior.cc for drawing agent position and
    // orientation.
    /*
    worldModel->getRVSender()->clear(); // erases drawings from previous cycle
    worldModel->getRVSender()->drawPoint("ball", ball.getX(), ball.getY(), 10.0f, RVSender::MAGENTA);
    */

    // ### Demo Behaviors ###

    // Walk in different directions
    //return goToTargetRelative(VecPosition(1,0,0), 0); // Forward
    //return goToTargetRelative(VecPosition(-1,0,0), 0); // Backward
    //return goToTargetRelative(VecPosition(0,1,0), 0); // Left
    //return goToTargetRelative(VecPosition(0,-1,0), 0); // Right
    //return goToTargetRelative(VecPosition(1,1,0), 0); // Diagonal
    //return goToTargetRelative(VecPosition(0,1,0), 90); // Turn counter-clockwise
    //return goToTargetRelative(VecPdosition(0,-1,0), -90); // Turn clockwise
    //return goToTargetRelative(VecPosition(1,0,0), 15); // Circle

    // Walk to the ball
    //return goToTarget(ball);

    // Turn in place to face ball
    /*double distance, angle;
    getTargetDistanceAndAngle(ball, distance, angle);
    if (abs(angle) > 10) {
      return goToTargetRelative(VecPosition(), angle);
    } else {
      return SKILL_STAND;
    }*/

    // Walk to ball while always facing forward
    //return goToTargetRelative(worldModel->g2l(ball), -worldModel->getMyAngDeg());

    // Dribble ball toward opponent's goal
    //return kickBall(KICK_DRIBBLE, VecPosition(HALF_FIELD_X, 0, 0));

    // Kick ball toward opponent's goal
    //return kickBall(KICK_FORWARD, VecPosition(HALF_FIELD_X, 0, 0)); // Basic kick
    //return kickBall(KICK_IK, VecPosition(HALF_FIELD_X, 0, 0)); // IK kick

    // Just stand in place
    //return SKILL_STAND;

    // Demo behavior where players form a rotating circle and kick the ball
    // back and forth
    

    VecPosition target = VecPosition(1.0,1.0,0);
    worldModel->getRVSender()->drawPoint("aaa",target.getX(),target.getY() ,12.0f,RVSender::ORANGE );
    VecPosition newTarget = getTargetPos(target);
    worldModel->getRVSender()->drawPoint("bbb",newTarget.getX(),newTarget.getY() ,12.0f,RVSender::RED );
    return goToTarget(newTarget);
//     VecPosition opp = worldModel->getOpponent(WO_OPPONENT1);
//          worldModel->getRVSender()->drawPoint("xxx",me.getX(),me.getY() ,10.0f,RVSender::GREEN );
//    if ( me.getDistanceTo(opp) < 2 ) {
// 	  lastPos = target;
// 	  VecPosition nextTarget = lastPos + VecPosition( 1.0 , 1.0 , 0);
// 	  
// 	  worldModel->getRVSender()->drawAgentText("xxx",RVSender::RED);
// 	  worldModel->getRVSender()->drawPoint("yyy",target.getX(),target.getY() ,10.0f,RVSender::RED );
// 	  // 如果busy，那么往前走。 如果free，那么转身往后到原来的地方
// 	  return goToTarget(  nextTarget );
//   }
//     else  return goToTarget( target );
//     return demoKickingCircle();
}


VecPosition NaoBehavior::getTargetPos(VecPosition targetPos){
	VecPosition opp = worldModel->getOpponent(WO_OPPONENT1);
	VecPosition epsilon =  VecPosition(1.0,1.0,0);
	stringstream ss;
	ss << lastPos.getX() << " " << lastPos.getY() ; 
	string text = ss.str();
	    worldModel->getRVSender()->drawAgentText(text,RVSender::RED);
	if ( abs( (lastPos - targetPos).getMagnitude() ) < 1e-2) {	// 已经挪过一次位置了
		VecPosition nextTarget = lastPos + epsilon;
		  worldModel->getRVSender()->drawAgentText("has",RVSender::RED);
		if ( me.getDistanceTo(opp) > 2 ) {   // 如果挪了位置后这个位置是空的，那么待在这个位置
		  return nextTarget;
		}else{   // 如果挪动以后还是有人
		    nextTarget = nextTarget + epsilon;		// 1. 再往前挪动一次
		    //return lastPos;		// 2. 回到原来的地方
		    // TODO: 如果紧急，那么往前走， 如果不急，退回原来的地方
		    return nextTarget;
		}
	}else{	// 未挪过位置
		if ( me.getDistanceTo(opp) < 2 ) {   // 被人盯防，需要挪动位置
			lastPos = targetPos;
			VecPosition nextTarget = lastPos + epsilon;
			return nextTarget;
	      }else  
		return targetPos; // 没有被盯住，那么当前这个位置是可以的
	}	

	return targetPos;
}

SkillType NaoBehavior::goToTargetDIng(VecPosition targetPos){
      VecPosition opp = worldModel->getOpponent(WO_OPPONENT1);
	if ( me.getDistanceTo(opp) < 2 ) {
		lastPos = targetPos;
		VecPosition nextTarget = lastPos + VecPosition( 1.0 , 1.0 , 0);
		
		worldModel->getRVSender()->drawAgentText("xxx",RVSender::RED);
		worldModel->getRVSender()->drawPoint("yyy",nextTarget.getX(),nextTarget.getY() ,10.0f,RVSender::RED );
		// 如果busy，那么往前走。 如果free，那么转身往后到原来的地方
		return goToTarget(  nextTarget );
	}
  
    return goToTarget(lastPos);
}

/*
 * Demo behavior where players form a rotating circle and kick the ball
 * back and forth
 */
SkillType NaoBehavior::demoKickingCircle() {
    // Parameters for circle
    VecPosition center = VecPosition(-HALF_FIELD_X/2.0, 0, 0);
    double circleRadius = 5.0;
    double rotateRate = 2.5;

    // Find closest player to ball
    int playerClosestToBall = -1;
    double closestDistanceToBall = 10000;
    for(int i = WO_TEAMMATE1; i < WO_TEAMMATE1+NUM_AGENTS; ++i) {
        VecPosition temp;
        int playerNum = i - WO_TEAMMATE1 + 1;
        if (worldModel->getUNum() == playerNum) {
            // This is us
            temp = worldModel->getMyPosition();
        } else {
            WorldObject* teammate = worldModel->getWorldObject( i );
            if (teammate->validPosition) {
                temp = teammate->pos;
            } else {
                continue;
            }
        }
        temp.setZ(0);

        double distanceToBall = temp.getDistanceTo(ball);
        if (distanceToBall < closestDistanceToBall) {
            playerClosestToBall = playerNum;
            closestDistanceToBall = distanceToBall;
        }
    }

    if (playerClosestToBall == worldModel->getUNum()) {
        // Have closest player kick the ball toward the center
        return kickBall(KICK_FORWARD, center);
    } else {
        // Move to circle position around center and face the center
        VecPosition localCenter = worldModel->g2l(center);
        SIM::AngDeg localCenterAngle = atan2Deg(localCenter.getY(), localCenter.getX());

        // Our desired target position on the circle
        // Compute target based on uniform number, rotate rate, and time
        VecPosition target = center + VecPosition(circleRadius,0,0).rotateAboutZ(360.0/(NUM_AGENTS-1)*(worldModel->getUNum()-(worldModel->getUNum() > playerClosestToBall ? 1 : 0)) + worldModel->getTime()*rotateRate);

        // Adjust target to not be too close to teammates or the ball
        target = collisionAvoidance(true /*teammate*/, false/*opponent*/, true/*ball*/, 1/*proximity thresh*/, .5/*collision thresh*/, target, true/*keepDistance*/);

        if (me.getDistanceTo(target) < .25 && abs(localCenterAngle) <= 10) {
            // Close enough to desired position and orientation so just stand
            return SKILL_STAND;
        } else if (me.getDistanceTo(target) < .5) {
            // Close to desired position so start turning to face center
            return goToTargetRelative(worldModel->g2l(target), localCenterAngle);
        } else {
            // Move toward target location
            return goToTarget(target);
        }
    }
}


