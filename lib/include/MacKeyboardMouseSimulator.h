#ifndef MKMS_MACKEYBOARDMOUSESIMULATOR_H_ZXBQBC6B
#define MKMS_MACKEYBOARDMOUSESIMULATOR_H_ZXBQBC6B

#include <list>
#include <string>
#include <vector>

#include "MacVirtualKeys.h"


namespace mkms
{

/**
 *  The MacKeyboardMouseSimulator class simulates key press/release mouse move
 *  and mouse click events with the help of mac OS-X Quartz Event Service 
 *  (http://developer.apple.com/library/mac/#documentation/Carbon/Reference/
 *    QuartzEventServicesRef/Reference/reference.html).
 *  The class uses the C API for event taps to alter the stream of low-level 
 *  user input events.
 *  
 *  Some parts are taken from: (http://github.com/doubleshow/libsikuli.git) 
 *  http://github.com/doubleshow/libsikuli/blob/master/src/robot.cpp.
 */
class MacKeyboardMouseSimulator
{
public:
  /**
   *  Ctor.
   */
  MacKeyboardMouseSimulator();
  
  /**
   *  Dtor.
   */
  ~MacKeyboardMouseSimulator();

  
  /**
   *  Structure to store a  key code and the string representation of this key 
   *  code. The structure provides a ctor to easily create an instance.
   */
  struct Key
  {
    /**
     *  The key code (have a look at MacVirtualKeys.h).
     */
    unsigned int keyCode;
    /**
     *  The string representation of the key.
     */
    std::string stringRepresentation;
    
    /**
     *  Key Ctor.
     * 
     *  @param pKeyCode The key code.
     *  @param pStringRepresentation The string representation of the key.
     */
    Key(unsigned int pKeyCode, std::string pStringRepresentation)
      : keyCode(pKeyCode),
        stringRepresentation(pStringRepresentation)
    {
    }
  };
  
  
  /**
   *  Returns a pointer to a standard vector of key structures which can be used
   *  to initialize a UI list.
   * 
   *  @return Standard vector of key structures.
   */
  std::vector<Key> * getKeyMap()
  {
    return mKeyMap;
  }
  
  /**
   *  Generates a key press and key release event for the given keycode.
   * 
   *  @param keyCode The keycode for the press/release event.
   */
  void pressOnce(unsigned int keyCode);
  
  /**
   *  Generates key release events for all keycodes which are NOT anymore in the 
   *  given keycode list compared to the "mLastKeyCodes" list and generates key 
   *  press events for all keycodes in the given list of keycodes.
   * 
   *  With this technique it is possible to create a constant stream of key 
   *  press events, so that we can controll games like UT or Quake3.
   * 
   *  @param keyCodes List of keycodes.
   */
  void press(std::list<unsigned int> keyCodes);
  
  /**
   *  Moves the mouse to the current mouse location + given x/y coordinates.
   * 
   *  @param x Relative x coordinate.
   *  @param y Relative y coordinate.
   */
  void mouseMove(int x, int y);
  
  /**
   *  Overloaded function to also pass double values to the mouse move function.
   *  
   *  @param x Relative x coordinate.
   *  @param y Relative y coordinate.
   */
  void mouseMove(double x, double y);
  
  /**
   *  Generates a single mouse click for the given button at the current 
   *  mouse pointer location. (1 == left mouse button, 4 == right mouse button)
   * 
   *  @param button Inidcates which mouse button
   */
  void mouseSingleClick(int button);
  
private:
  // Generates a key press event.
  void keyPress(unsigned int keyCode);
  // Generates a key release event.
  void keyRelease(unsigned int keyCode);

  // Generates a key release event for every given key code in the list.
  void releaseKeys(std::list<unsigned int> keyCodes);
  // Returns the difference btw the stored mLastKeyCodes and the given list of 
  // keycodes
  std::list<unsigned int> findDifference(std::list<unsigned int> keyCodes);

  // Converts the given user screen id to the mac display id. For now we always 
  // use 1 as user screen ID.
  int convertUserScreenIdToMacDisplayId(int screen);
  // Stores the screen bounds of the given screen ID into the given parameter.
  void getScreenBounds(int screen, int &x, int &y, int &w, int &h);
  // Moves the mouse from the the given x0/y0 position to the given x1/y1 
  // position with a step size of 50. (absolute position)
  void mouseMoveFromTo(int x0, int y0, int x1, int y1);
  // Generates a mouse move event (absolute position).
  void mouseMoveTo(int x, int y);
  // Generates a mouse press event for the given button.
  void mousePress(int button);
  // Generates a mouse release event for the given button.
  void mouseRelease(int button);

  // prints out (with std::cout) the given list of unsigned integers.
  void debugList(std::list<unsigned int> list);

  // Initializes a keymap which stores all possible key codes to string 
  // representations.
  void setupKeyMap();
  
  // Map of key structures.
  std::vector<Key> *mKeyMap;
  // List of last used keycodes.
  std::list<unsigned int> *mLastKeyCodes;
};

} /* mkms */

#endif /* end of include guard: MKMS_MACKEYBOARDMOUSESIMULATOR_H_ZXBQBC6B */
