#include "naobehavior.h"
#include "../rvdraw/rvdraw.h"

#include <time.h>

extern int agentBodyType;


// VecPosition randomBeamPos(){
//    double a = -2.0f;
//    double b = 2.0f;
//    srand((int)time(0));
//    double x = ((double)rand()/RAND_MAX)*((0.0f)-(-2.0f)) + (-2.0f);
//    double y = ((double)rand()/RAND_MAX)*(b-a) + a;
//    //cout << "x" << x << '\t' << "y" << y << endl;
//    return VecPosition(x,y,0);
// }
//   


/*
 * Real game beaming.
 * Filling params x y angle
 */
void NaoBehavior::beam( double& beamX, double& beamY, double& beamAngle ) {
   //VecPosition BeamPos = randomBeamPos();   
//     beamX = -HALF_FIELD_X;
//     beamY = 0.0f;
   beamX = -3.0f;
   beamY =    -HALF_FIELD_Y ;
   beamAngle = +90;
//    beamAngle = ((double)rand()/RAND_MAX)*((360.0f)-(-0.0f)) + (-0.0f);
}


bool NaoBehavior::Valid(int player,VecPosition off){
    WorldObject* teammate = worldModel->getWorldObject( player );
    VecPosition temp = teammate->pos;
    if (!teammate->validPosition) {  //not out of range..out of the filed....mostly to test fouls
        return false; 
    } 
    else{
        for(int i=WO_OPPONENT1;i<WO_OPPONENT1+NUM_AGENTS;i++) {      
                VecPosition attposition = worldModel->getOpponent(i);
                double angletodefender = attposition.getAngleWithVector(off);
		cout << angletodefender << '\t' << temp.getAngleWithVector(off) << endl;
            if( temp.getAngleWithVector(off)+3 > angletodefender &&  temp.getAngleWithVector(off)-3 < angletodefender )
                return false;
        }
        if(temp.getDistanceTo(off)>14) return false;
        else  return true;
    }
}

/* Checks if the ball is currently moving */
bool NaoBehavior::isBallMoving(const WorldModel *worldModel) {
    static VecPosition lastBall = worldModel->getBallGroundTruth();
    static double lastTime = worldModel->getTime();

    double thisTime = worldModel->getTime();
    VecPosition thisBall = worldModel->getBallGroundTruth();

    thisBall.setZ(0);
    lastBall.setZ(0);

    if(thisBall.getDistanceTo(lastBall) > 0.01)
    {
        // the ball moved!
        lastBall = thisBall;
        lastTime = thisTime;
        return true;
    }

    if(thisTime - lastTime < 0.5)
    {
        // not sure yet if the ball has settled
        return true;
    }
    else
    {
        return false;
    }
}



SkillType NaoBehavior::selectSkill() {
//   VecPosition target(HALF_FIELD_X,0,0);
  VecPosition target( -3 ,  HALF_FIELD_Y  ,0);
    if( worldModel->getUNum() == WO_TEAMMATE1) {
      worldModel->getRVSender()->drawPoint( me.getX() , me.getY() , 4.0 , RVSender::YELLOW);
      if ( me.getDistanceTo(target)  < 1 ) {
 	if (!oneTimeTest ){
		 oneTimeTest = true;
		string msg = "(playMode GameOver)";
		setMonMessage(msg);
 	}
	return SKILL_STAND;
      }
      else return goToTarget( target );
    }
    return SKILL_STAND;
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


