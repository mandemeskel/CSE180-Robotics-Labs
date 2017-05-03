#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <string>
#include <geometry_msgs/Pose2D.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <random_numbers/random_numbers.h>

using namespace std;

#define NODE_NAME "navigatepoints"
#define TOPIC_NAME "move_base"

// current husky position
geometry_msgs::Pose2D currentLocation;

// for getting next random position
random_numbers::RandomNumberGenerator* rng = new random_numbers::RandomNumberGenerator();

move_base_msgs::MoveBaseGoal * createGoal( int, int, float );
string testState( actionlib::SimpleClientGoalState );

int main(int argc,char **argv) {

    ros::init( argc, argv, NODE_NAME );
    ros::NodeHandle nh;

    actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction>
	ac( TOPIC_NAME, true );
    ROS_INFO_STREAM("Waiting for server to be available...");
    while ( !ac.waitForServer() ) {

    }
    ROS_INFO_STREAM("done!");

    // get current location
    nh..subscribe( "/odom/filtered", 10, odometryHandler );

    // create goal to send to the bot
    // this goal fails because theta is 0.0:
    // [ERROR] [1488523621.229798856, 15.870000000]: Quaternion has length close to zero... discarding as navigation goal
    // [ERROR] [1488523630.631711562, 25.230000000]: Quaternion has length close to zero... discarding as navigation goal
    move_base_msgs::MoveBaseGoal * goal;

    // new goal
    while( true ) {
        
        ros.spinOnce();
        goal = createGoal( 0, 0, -90.0 );
        ac.sendGoal( *goal );
        ac.waitForResult();
        ROS_INFO_STREAM( testState( ac.getState() ) );
        delete goal;
        
    }
    
    return 0;

}


move_base_msgs::MoveBaseGoal * createGoal( int x, int y, float angle ) {
    
    move_base_msgs::MoveBaseGoal * goal = new move_base_msgs::MoveBaseGoal();

    goal->target_pose.header.frame_id = "map";
    goal->target_pose.header.stamp = ros::Time::now();
    
    //goal->target_pose.pose.position.x = x;
    //goal->target_pose.pose.position.y = y;
    //goal->target_pose.pose.orientation.w = angle;

    // randomly generated goal
    // TODO: check with global_costmap to see if this is an open cell
    goal->theta = rng->gaussian(currentLocation.theta, 0.25);
    goal->x = currentLocation.x + (0.5 * cos(goalLocation.theta));
    goal->y = currentLocation.y + (0.5 * sin(goalLocation.theta));

    return goal;

}


string testState( actionlib::SimpleClientGoalState state ) {

    string out;

    if( state == actionlib::SimpleClientGoalState::SUCCEEDED )
	    out = "Success";
    else
	    out = "Failure";

    return out;

}

void odometryHandler(const nav_msgs::Odometry::ConstPtr& message) {
    //Get (x,y) location directly from pose
    currentLocation.x = message->pose.pose.position.x;
    currentLocation.y = message->pose.pose.position.y;

    //Get theta rotation by converting quaternion orientation to pitch/roll/yaw
    tf::Quaternion q(message->pose.pose.orientation.x, message->pose.pose.orientation.y, message->pose.pose.orientation.z, message->pose.pose.orientation.w);
    tf::Matrix3x3 m(q);
    double roll, pitch, yaw;
    m.getRPY(roll, pitch, yaw);
    currentLocation.theta = yaw;
}


