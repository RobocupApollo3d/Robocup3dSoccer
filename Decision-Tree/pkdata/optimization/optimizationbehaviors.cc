#include "optimizationbehaviors.h"
#include <fstream>


/*
 *
 *
 * Fixed Kick optimization agent
 *
 *
 */
OptimizationBehaviorFixedKick::OptimizationBehaviorFixedKick(const std::string teamName,
        int uNum, const map<string, string>& namedParams_, const string& rsg_,
        const string& outputFile_) :
    NaoBehavior(teamName, uNum, namedParams_, rsg_), outputFile(outputFile_),
    kick(0), INIT_WAIT_TIME(3.0)
{
    initKick();
}

void OptimizationBehaviorFixedKick::beam(double& beamX, double& beamY,
        double& beamAngle) {
    beamX = atof(namedParams.find("kick_xoffset")->second.c_str());
    beamY = atof(namedParams.find("kick_yoffset")->second.c_str());
    beamAngle = atof(namedParams.find("kick_angle")->second.c_str());
}

SkillType OptimizationBehaviorFixedKick::selectSkill() {
    double time = worldModel->getTime();
    if (timeStart < 0) {
        initKick();
        return SKILL_STAND;
    }

    // Wait a bit before attempting kick
    if (time-timeStart <= INIT_WAIT_TIME) {
        return SKILL_STAND;
    }

    if (!hasKicked) {
        hasKicked = true;
        return SKILL_KICK_LEFT_LEG; // The kick skill that we're optimizing
    }

    return SKILL_STAND;
}

void OptimizationBehaviorFixedKick::updateFitness() {
    static double totalFitness = 0.0;
    if (kick == 10) {
        writeFitnessToOutputFile(totalFitness/(double(kick)));
        return;
    }

    double time = worldModel->getTime();
    VecPosition meTruth = worldModel->getMyPositionGroundTruth();
    meTruth.setZ(0);

    if (time-timeStart <= INIT_WAIT_TIME) {
        return;
    }

    static bool failedLastBeamCheck = false;
    if(!beamChecked) {
        cout << "Checking whether beam was successful\n";
        beamChecked = true;
        LOG_STR("Checking whether beam was successful");

        meTruth.setZ(0);
        double beamX, beamY, beamAngle;
        beam(beamX, beamY, beamAngle);
        VecPosition meDesired = VecPosition(beamX, beamY, 0);
        double distance = meTruth.getDistanceTo(meDesired);
        double angle = worldModel->getMyAngDegGroundTruth();
        VecPosition ballPos = worldModel->getBallGroundTruth();
        double ballDistance = ballPos.getMagnitude();

        // Check that we're close to our expected position and angle
        // and also that the ball is close to it's exepected position
        if(distance > .1 || ballDistance > .1 || abs(angle - beamAngle) > 3) {
            cout << distance << "\t" << ballDistance << "\n";
            LOG_STR("Problem with the beam!");
            LOG(distance);
            LOG(meTruth);
            LOG(meDesired);
            if (failedLastBeamCheck) {
                kick++;
                totalFitness -= 100;
                failedLastBeamCheck = false;
            } else {
                failedLastBeamCheck = true;
            }
            initKick();
            return;
        }
        failedLastBeamCheck = false;
        string msg = "(playMode KickOff_Left)";
        setMonMessage(msg);
    }

    if (!hasKicked && worldModel->getPlayMode() == PM_PLAY_ON) {
        ranIntoBall = true;
    }

    if (!hasKicked) {
        return;
    }

    VecPosition ballTruth = worldModel->getBallGroundTruth();
    if (ballTruth.getX() < -.25) {
        backwards = true;
    }

    if (worldModel->isFallen()) {
        fallen = true;
    }

    if (worldModel->isFallen()) {
        totalFitness += -1;
        kick++;
        initKick();
        return;
    }

    if (time - (timeStart + INIT_WAIT_TIME) > 15 && !isBallMoving(this->worldModel)) {
        double angleOffset = abs(VecPosition(0, 0, 0).getAngleBetweenPoints(VecPosition(20, 0, 0), ballTruth));
        double distance = ballTruth.getX();
        double fitness = distance;

        if (backwards || distance <= 0.1 || ranIntoBall) {
            fitness = -100;
            if (backwards) {
                cout << "Detected backward kick" << endl;
            } else if (ranIntoBall) {
                cout << "Detected ranIntoBall" << endl;
            } else {
                cout << "Detected insufficient distance" << endl;
            }
        }
        cout << "Traveled distance = " << distance << endl;
        cout << "Fitness = " << fitness << endl;
        cout << "Final position = " << ballTruth.getX() << ", " << ballTruth.getY() << endl;

        totalFitness += fitness;
        kick++;
        initKick();
        return;
    }
}

void OptimizationBehaviorFixedKick::initKick() {
    hasKicked = false;
    beamChecked = false;
    backwards = false;
    ranIntoBall = false;
    timeStart = worldModel->getTime();
    initialized = false;
    initBeamed = false;
    fallen = false;
    resetSkills();

    // Beam agent and ball
    double beamX, beamY, beamAngle;
    beam(beamX, beamY, beamAngle);
    VecPosition beamPos = VecPosition(beamX, beamY, 0);
    string msg = "(playMode BeforeKickOff)";
    setMonMessage(msg);
}

void OptimizationBehaviorFixedKick::writeFitnessToOutputFile(double fitness) {
    static bool written = false;
    if (!written) {
        LOG(fitness);
        LOG(kick);
        fstream file;
        file.open(outputFile.c_str(), ios::out);
        file << fitness << endl;
        file.close();
        written = true;
        //string msg = "(killsim)";
        //setMonMessage(msg);
    }
}


/* Checks if the ball is currently moving */
bool isBallMoving(const WorldModel *worldModel) {
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


void writeToOutputFile(const string &filename, const string &output) {
//  static bool written = false;
//  assert(!written);
    //  LOG(output);
    fstream file;
    file.open(filename.c_str(), ios::out);
    file << output;
    file.close();
//  written = true;
}


/*
 *
 *
 * WALK FORWARD OPTIMIZATION AGENT
 *
 *
 *
 */
OptimizationBehaviorWalkForward::
OptimizationBehaviorWalkForward( const std::string teamName,
                                 int uNum,
                                 const map<string, string>& namedParams_,
                                 const string& rsg_,
                                 const string& outputFile_)
    : NaoBehavior( teamName,
                   uNum,
                   namedParams_,
                   rsg_ ),
    outputFile( outputFile_ ) {


    INIT_WAIT = 3;
    run = 0;
    totalWalkDist = 0;

    // Use ground truth localization for behavior
    worldModel->setUseGroundTruthDataForLocalization(true);

    init();
}

void OptimizationBehaviorWalkForward::init() {
    startTime = worldModel->getTime();
    initialized = false;
    initBeamed = false;
    beamChecked = false;
    string msg = "(playMode BeforeKickOff)";
    setMonMessage(msg);
}

void OptimizationBehaviorWalkForward::
beam( double& beamX, double& beamY, double& beamAngle ) {
    beamX = -HALF_FIELD_X+3;
    beamY = 0;
    beamAngle = 0;
}

bool OptimizationBehaviorWalkForward::checkBeam() {
    LOG_STR("Checking whether beam was successful");
    VecPosition meTruth = worldModel->getMyPositionGroundTruth();
    meTruth.setZ(0);
    double beamX, beamY, beamAngle;
    beam(beamX, beamY, beamAngle);
    VecPosition meDesired = VecPosition(beamX, beamY, 0);
    double distance = meTruth.getDistanceTo(meDesired);
    double angleOffset = abs(worldModel->getMyAngDegGroundTruth()-beamAngle);
    if(distance > 0.05 || angleOffset > 5) {
        LOG_STR("Problem with the beam!");
        LOG(distance);
        LOG(meTruth);
        return false;
    }
    beamChecked = true;
    return true;
}

SkillType OptimizationBehaviorWalkForward::
selectSkill() {
    double currentTime = worldModel->getTime();
    if (currentTime-startTime < INIT_WAIT || startTime < 0) {
        return SKILL_STAND;
    }

    return goToTarget(VecPosition(HALF_FIELD_X, 0, 0));
}

void OptimizationBehaviorWalkForward::
updateFitness() {
    static bool written = false;

    if (run == 10) {
        if (!written) {
            double fitness = totalWalkDist/(double)run;
            fstream file;
            file.open(outputFile.c_str(), ios::out );
            file << fitness << endl;
            file.close();
            written = true;
        }
        return;
    }

    if (startTime < 0) {
        init();
        return;
    }

    double currentTime = worldModel->getTime();
    if (currentTime-startTime < INIT_WAIT) {
        return;
    }

    if (!beamChecked) {
        static bool failedLastBeamCheck = false;
        if (!checkBeam()) {
            // Beam failed so reinitialize everything
            if (failedLastBeamCheck) {
                // Probably something bad happened if we failed the beam twice in
                // a row (perhaps the agent can't stand) so give a bad score and
                // move on
                totalWalkDist -= 100;
                run++;
            }
            failedLastBeamCheck = true;
            init();
            return;
        } else {
            failedLastBeamCheck = false;
            // Set playmode to PlayOn to start run and move ball out of the way
            string msg = "(playMode PlayOn) (ball (pos 0 -9 0) (vel 0 0 0))";
            setMonMessage(msg);
        }
    }

    if (currentTime-startTime >= 10.0+INIT_WAIT) {
        VecPosition me = worldModel->getMyPositionGroundTruth();
        double beamX, beamY, beamAngle;
        beam(beamX, beamY, beamAngle);
        VecPosition start = VecPosition(beamX, beamY, 0);

        double walkdist = (me-start).getX();
        cout << "Run " << run << " distance walked: " << walkdist << endl;
        totalWalkDist += walkdist;
        run++;
        init();
    }
}





/*
 *
 *
 * Fixed Kick optimization agent
 *
 *
 */
VecPosition randomBeamPos(){
     double thod = 2.0f;
     double x=0.0f;
     double y=0.0f;

    do{
      srand((int)time(0));
      x = ((double)rand()/RAND_MAX)*((thod)-(-thod)) + (-thod);
      y = ((double)rand()/RAND_MAX)*(thod-(-thod)) + (-thod);
    }while( ( x < 0.3f && x > -0.3f ) || ( y < 0.3f && y > -0.3f ) );
   return VecPosition(x,y,0);
}

double getrandomAngle(VecPosition v){
       double angle = 0.0f;
       double x = v.getX();
       double y = v.getY();
    if ( x < 0 && y > 0){
        angle = ((double)rand()/RAND_MAX)*((-90.0f)-(-180.0f)) + (-180.0f);
    }else if ( x < 0 && y < 0){
        angle = ((double)rand()/RAND_MAX)*((0.0f)-(-90.0f)) + (-90.0f);
    }else
      angle = ((double)rand()/RAND_MAX)*((180.0f)-(0.0f)) + (0.0f);
    return angle;
}

string int2string(double a){
   stringstream s;
   s << a;
   return s.str();
}


OptimizationKickModel::OptimizationKickModel(const std::string teamName,
        int uNum, const map<string, string>& namedParams_, const string& rsg_,
        const string& outputFile_ ) :
    NaoBehavior(teamName, uNum, namedParams_, rsg_), outputFile(outputFile_),
    kick(0), INIT_WAIT_TIME(3.0)
{
    trainType= SKILL_KICK_LOW_LEFT_LEG;
    // 修改要预测的踢球类型
    initKick();
}

void OptimizationKickModel::beam(double& beamX, double& beamY,
        double& beamAngle) {
	  if (trainType ==  SKILL_KICK_LOW_LEFT_LEG || trainType == SKILL_KICK_LOW_RIGHT_LEG){
	      beamX = atof(namedParams.find("kick_low_xoffset")->second.c_str());
	      beamY = atof(namedParams.find("kick_low_yoffset")->second.c_str());
	      beamAngle = atof(namedParams.find("kick_low_angle")->second.c_str());
	  }
	  else  if (trainType ==  SKILL_KICK_6M_LEFT_LEG || trainType == SKILL_KICK_6M_RIGHT_LEG){
	      beamX = atof(namedParams.find("kick_6m_xoffset")->second.c_str());
	      beamY = atof(namedParams.find("kick_6m_yoffset")->second.c_str());
	      beamAngle = atof(namedParams.find("kick_6m_angle")->second.c_str());
	  }
	 else    if (trainType ==  SKILL_KICK_8M_LEFT_LEG || trainType == SKILL_KICK_8M_RIGHT_LEG){
	      beamX = atof(namedParams.find("kick_8m_xoffset")->second.c_str());
	      beamY = atof(namedParams.find("kick_8m_yoffset")->second.c_str());
	      beamAngle = atof(namedParams.find("kick_8m_angle")->second.c_str());
	  }
	 else if (trainType ==  SKILL_KICK_FAST_LEFT_LEG || trainType == SKILL_KICK_FAST_RIGHT_LEG){
	      beamX = atof(namedParams.find("kick_fast_xoffset")->second.c_str());
	      beamY = atof(namedParams.find("kick_fast_yoffset")->second.c_str());
	      beamAngle = atof(namedParams.find("kick_fast_angle")->second.c_str());
	  }
	else   if (trainType ==  SKILL_KICK_9M_LEFT_LEG || trainType == SKILL_KICK_9M_RIGHT_LEG){
	      beamX = atof(namedParams.find("kick_9m_xoffset")->second.c_str());
	      beamY = atof(namedParams.find("kick_9m_yoffset")->second.c_str());
	      beamAngle = atof(namedParams.find("kick_9m_angle")->second.c_str());
	  }
	else      if (trainType ==  SKILL_KICK_11M_LEFT_LEG || trainType == SKILL_KICK_11M_RIGHT_LEG){
	      beamX = atof(namedParams.find("kick_11m_xoffset")->second.c_str());
	      beamY = atof(namedParams.find("kick_11m_yoffset")->second.c_str());
	      beamAngle = atof(namedParams.find("kick_low_angle")->second.c_str());
	  }
	 else{
	      beamX = atof(namedParams.find("kick_xoffset")->second.c_str());
	      beamY = atof(namedParams.find("kick_yoffset")->second.c_str());
	      beamAngle = atof(namedParams.find("kick_angle")->second.c_str());
	 }

}



SkillType OptimizationKickModel::selectSkill() {
    double time = worldModel->getTime();
    if (timeStart < 0) {
        cout << "timeStart < 0 "<< endl;
        initKick();
        return SKILL_STAND;
    }

    // Wait a bit before attempting kick
    if (time-timeStart <= INIT_WAIT_TIME) {
        return SKILL_STAND;
    }



    WorldObject *opp = worldModel->getWorldObject(WO_OPPONENT11);

        cerr << "----=-==-" << endl;
        cerr << "getZ()" << opp->pos.getZ()  << endl;
        cerr << "----=-==-" << endl;
   //if ( time - (timeStart + INIT_WAIT_TIME) > 6.5f && ( opp->pos.getZ() > 0.15f && opp->pos.getZ() < 0.30f)  ){
    if ( time - (timeStart + INIT_WAIT_TIME) > 6 && ( opp->pos.getZ() > 0.2f && opp->pos.getZ() < 0.30f)  ){
      // 一个是每回合踢球的时间即每回合第4s后开始 ， 第二个约束条件是 对方球员不是摔倒 及 空中掉下两种情况
          if (!hasKicked) {
        hasKicked = true;

    cout << "BallDist2me " << worldModel->getBallGroundTruth().getDistanceTo(ball) << endl;



    VecPosition localCenter = worldModel->g2l(ball);
        SIM::AngDeg localCenterAngle = atan2Deg(localCenter.getY(), localCenter.getX());

    cout << "OPP1angle " << localCenterAngle << endl;
    cout << "OPP1VelMagnitude " << opp->absVel.getMagnitude() << endl;
    //cout << "OPP1angle " << opp->pos.getAngleWithVector(ball) << endl;


    // 训练agent的类型
    dataarr.push_back(trainType);
    // 踢球类型
    dataarr.push_back(opp->pos.getDistanceTo(ball)) ;
    // 对方离球距离
    dataarr.push_back(localCenterAngle) ;
    // 对方与球角度
    dataarr.push_back(opp->absVel.getMagnitude()) ;
    // 对方速度的模

	return trainType;
	// 踢球是延时阻塞的，而到某个地方是瞬时可变的，具体参看kickball执行过程
      }
    }
   return SKILL_STAND;
}



void OptimizationKickModel::updateFitness() {
    static double totalFitness = 0.0;

    int beishu= 100;
    // 最先的是    SKILL_KICK_LOW_LEFT_LEG
    if (kick == 12*beishu) {
        return;
    }    else if (kick == 1*beishu){
	  trainType =  SKILL_KICK_LOW_RIGHT_LEG;
	  cout << "当前踢球类型改变为" << trainType << endl ;
    }    else if (kick == 2*beishu){
	    trainType = SKILL_KICK_6M_LEFT_LEG;
            cout << "当前踢球类型改变为" << trainType << endl ;
    }    else if (kick == 3*beishu){
	    trainType =SKILL_KICK_6M_RIGHT_LEG;
            cout << "当前踢球类型改变为" << trainType << endl ;
    }    else if (kick == 4*beishu){
	   trainType = SKILL_KICK_8M_LEFT_LEG;
            cout << "当前踢球类型改变为" << trainType << endl ;
    }    else if (kick == 5*beishu){
	    trainType = SKILL_KICK_8M_RIGHT_LEG;
            cout << "当前踢球类型改变为" << trainType << endl ;
    }    else if (kick == 6*beishu){
	    trainType = SKILL_KICK_FAST_LEFT_LEG;
            cout << "当前踢球类型改变为" << trainType << endl ;
    }    else if (kick == 7*beishu){
	    trainType = SKILL_KICK_FAST_RIGHT_LEG;
            cout << "当前踢球类型改变为" << trainType  << endl;
    }    else if (kick == 8*beishu){
	    trainType = SKILL_KICK_9M_LEFT_LEG;
            cout << "当前踢球类型改变为" << trainType  << endl;
    }    else if (kick == 9*beishu){
	    trainType = SKILL_KICK_9M_RIGHT_LEG;
            cout << "当前踢球类型改变为" << trainType  << endl;
    }    else if (kick == 10*beishu){
	    trainType = SKILL_KICK_11M_LEFT_LEG;
            cout << "当前踢球类型改变为" << trainType  << endl;
    }    else if (kick == 11*beishu){
	    trainType = SKILL_KICK_11M_RIGHT_LEG;
            cout << "当前踢球类型改变为" << trainType << endl ;
    }


    double time = worldModel->getTime();
    VecPosition meTruth = worldModel->getMyPositionGroundTruth();
    meTruth.setZ(0);

    if (time-timeStart <= INIT_WAIT_TIME) {
        return;
    }

    static bool failedLastBeamCheck = false;
    if(!beamChecked) {
        cout << "Checking whether beam was successful\n";
        beamChecked = true;
        LOG_STR("Checking whether beam was successful");

        meTruth.setZ(0);
        double beamX, beamY, beamAngle;
        beam(beamX, beamY, beamAngle);

        VecPosition meDesired = VecPosition(beamX, beamY, 0);
        double distance = meTruth.getDistanceTo(meDesired);
        double angle = worldModel->getMyAngDegGroundTruth();

        VecPosition ballPos = worldModel->getBallGroundTruth();
        double ballDistance = ballPos.getMagnitude();

        // Check that we're close to our expected position and angle
        // and also that the ball is close to it's exepected position
        if(distance > .1 || ballDistance > .1 || abs(angle - beamAngle) > 3) {
            cout << distance << "\t" << ballDistance << "\n";
            LOG_STR("Problem with the beam!");
            LOG(distance);
            LOG(meTruth);
            LOG(meDesired);
            if (failedLastBeamCheck) {
                kick++;
                totalFitness -= 100;
                failedLastBeamCheck = false;
            } else {
                failedLastBeamCheck = true;
            }
            initKick();
            return;
        }
        failedLastBeamCheck = false;
    VecPosition randomP = randomBeamPos();
    double ang = getrandomAngle(randomP);
    string x = int2string(randomP.getX());
    string y = int2string(randomP.getY());
    string angstr = int2string(ang);

    cout << "--------开始定位------" << endl;
    cout << "随机地点的X：" << x << '\t'  << "Y："<<y  << endl;
    string msg = "(playMode PlayOn)(agent (unum 11) (team Right) (move " + x +" " + y + " " + "0" + " "+ angstr + "))";
    // 这边平台指令必须合并成为一句

    setMonMessage(msg);
    }


    if (!hasKicked && worldModel->getPlayMode() == PM_PLAY_ON) {
        ranIntoBall = true;
    }

    VecPosition ballTruth = worldModel->getBallGroundTruth();
    if (ballTruth.getX() < -.25) {
        backwards = true;
    }

    if (time - (timeStart + INIT_WAIT_TIME) > 12 && !isBallMoving(this->worldModel)) {
        double angleOffset = abs(VecPosition(0, 0, 0).getAngleBetweenPoints(VecPosition(20, 0, 0), ballTruth));
        double distance = ballTruth.getX();
//         double fitness = distance;

//         if (backwards || distance <= 0.1f || ranIntoBall) {
//             fitness = -100;
//             if (backwards) {
//                 cout << "Detected backward kick" << endl;
//             } else if (ranIntoBall) {
//                 cout << "Detected ranIntoBall" << endl;
//             } else {
//                 cout << "Detected insufficient distance" << endl;
//             }
//         }

        cout << "Traveled distance = " << distance << endl;
//         cout << "Fitness = " << fitness << endl;
        cout << "Final position = " << ballTruth.getX() << ", " << ballTruth.getY() << endl;

    if ( hasKicked ){
          if ( ballTruth.getX()  < 0.2f ){
	      dataarr.push_back(0.0f);
	  }else if (distance < 0.4f){
	      dataarr.push_back(0.0f);
	  }else{
	    dataarr.push_back(1.0f);
	  }
          writePKdata();
    }else{
            cout << "\n=-------写入失败,未踢球-------=" << endl;
    }


        cout << "=-------一轮结束-------=\n" << endl;

//         totalFitness += fitness;
        kick++;


        initKick();	 // 进行下一回合的优化
        return;
    }
}

void OptimizationKickModel::initKick() {
  cout << "initkick" << endl;
    hasKicked = false;
    beamChecked = false;
    backwards = false;
    ranIntoBall = false;
    timeStart = worldModel->getTime();
    initialized = false;
    initBeamed = false;
    fallen = false;
    resetSkills();

    // Beam agent and ball
    //double beamX, beamY, beamAngle;
    //beam(beamX, beamY, beamAngle);
    //VecPosition beamPos = VecPosition(beamX, beamY, 0);

    //string msg = "(playMode PlayOn)";
    //string msg = "(playMode BeforeKickOff)";
    //string msg  = "(playMode PlayOn) (ball (pos 0 -9 0) (vel 0 0 0))";
    string msg = "(playMode BeforeKickOff)";
    setMonMessage(msg);

}



void OptimizationKickModel::writePKdata() {
        string pkdata = "pkdata.csv";
        fstream file;
        file.open(pkdata.c_str(), ios::app);

    for(vector<double>::iterator it = dataarr.begin() ; it != dataarr.end(); it ++){
      cout <<  *it << ",";
      file << *it << ",";
    }

    file << endl;
    dataarr.clear();
    file.close();

    cout << "\n=-------写入成功-------=" << endl;
}



void OptimizationKickModel::writeFitnessToOutputFile(double fitness) {
    static bool written = false;
    if (!written) {
        LOG(fitness);
        LOG(kick);
        fstream file;
        file.open(outputFile.c_str(), ios::out);
        file << fitness << endl;
        file.close();
        written = true;
        //string msg = "(killsim)";
        //setMonMessage(msg);
    }
}
