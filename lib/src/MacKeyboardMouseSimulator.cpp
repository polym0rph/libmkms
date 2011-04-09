#include <algorithm>
#include <iostream>

#include "ApplicationServices/ApplicationServices.h"

#include "MacKeyboardMouseSimulator.h"


namespace mkms
{

#define MOUSE_BUTTON1_MASK 1
#define MOUSE_BUTTON2_MASK 2  
#define MOUSE_BUTTON3_MASK 4


MacKeyboardMouseSimulator::MacKeyboardMouseSimulator()
  : mKeyMap(0),
    mLastKeyCodes(new std::list<unsigned int>())
{
  setupKeyMap();
}

MacKeyboardMouseSimulator::~MacKeyboardMouseSimulator()
{
  delete(mKeyMap);
  delete(mLastKeyCodes);
}

void MacKeyboardMouseSimulator::pressOnce(unsigned int pKeyCode)
{
  keyPress(pKeyCode);
  keyRelease(pKeyCode);
}

void MacKeyboardMouseSimulator::press(std::list<unsigned int> pKeyCodes)
{
  if(pKeyCodes.empty())
  {
    // release all keys which are still in the last key code list
    if(!mLastKeyCodes->empty())
      releaseKeys(*mLastKeyCodes);
  }
  else
  {
    // release all keys which are NOT in the given key code list compared to 
    // mLastKeyCodes list
    releaseKeys(findDifference(pKeyCodes));
    
    // press all keys in the given list
    std::list<unsigned int>::iterator it;
    for(it = pKeyCodes.begin(); it != pKeyCodes.end(); ++it)
      keyPress(*it);
    
    // copy the new list of key codes
    mLastKeyCodes->resize(pKeyCodes.size());
    copy(pKeyCodes.begin(), pKeyCodes.end(), mLastKeyCodes->begin());
  }
}

void MacKeyboardMouseSimulator::mouseMove(int pX, int pY)
{
  CGPoint curloc;
  CGEventRef eventRef;
  
  eventRef = CGEventCreate(NULL);
  curloc = CGEventGetLocation(eventRef);
  CFRelease(eventRef);
  
  int x0, y0, w0, h0;
  // first parameter is the screen id
  getScreenBounds(0, x0, y0, w0, h0);
  
  pX += x0;
  pY += y0;
  
  // std::cout << "current location: x: " << curloc.x << " y: " << curloc.y
  //   << std::endl;
  // std::cout << "screen bounds: x: " << x0 << " y: " << y0 << " w: " << w0
  //   << " h: " << h0 << std::endl;
  // std::cout << "position - x: " << pX << " y: " << pY << std::endl;  
  
  mouseMoveFromTo(curloc.x, curloc.y, pX, pY);
}

void MacKeyboardMouseSimulator::mouseMove(double pX, double pY)
{
  // TODO: find a solution to not call getScreenBounds a second time
  int x0, y0, w0, h0;
  // first parameter is the screen id
  getScreenBounds(0, x0, y0, w0, h0);
  
  mouseMove(int(pX * w0), int(pY * h0));
}

void MacKeyboardMouseSimulator::mouseSingleClick(int pButton)
{
  mousePress(pButton);
  mouseRelease(pButton);
}


void MacKeyboardMouseSimulator::keyPress(unsigned int pKeyCode)
{  
  CGEventRef eventRef;
  eventRef = CGEventCreateKeyboardEvent (NULL, (CGKeyCode)pKeyCode, true);
  CGEventPost(kCGSessionEventTap, eventRef);
  CFRelease(eventRef);
}

void MacKeyboardMouseSimulator::keyRelease(unsigned int pKeyCode)
{  
  CGEventRef eventRef;
  eventRef = CGEventCreateKeyboardEvent (NULL, (CGKeyCode)pKeyCode, false);
  CGEventPost(kCGSessionEventTap, eventRef);
  CFRelease(eventRef);
}

void MacKeyboardMouseSimulator::releaseKeys(std::list<unsigned int> pKeyCodes)
{
  std::list<unsigned int>::iterator it;
  for(it = pKeyCodes.begin(); it != pKeyCodes.end(); ++it)
    keyRelease(*it);
  
  mLastKeyCodes->clear();
}

std::list<unsigned int> MacKeyboardMouseSimulator::findDifference(
  std::list<unsigned int> pKeyCodes)
{
  std::list<unsigned int> keysToRelease;
  
  std::list<unsigned int>::iterator keyCodeIt;
  std::list<unsigned int>::iterator lastKeyCodeIt;
  
  // loop through the list of the previous keycodes and
  // find the keycodes which are not in the current list
  for(lastKeyCodeIt = mLastKeyCodes->begin();
      lastKeyCodeIt != mLastKeyCodes->end(); ++lastKeyCodeIt)
  {
    keyCodeIt = std::find(pKeyCodes.begin(), pKeyCodes.end(), (*lastKeyCodeIt));
    
    if(keyCodeIt == pKeyCodes.end())
      keysToRelease.push_back(*lastKeyCodeIt);
  }
  
  return keysToRelease;
}

int MacKeyboardMouseSimulator::convertUserScreenIdToMacDisplayId(
  int pScreen)
{
  CGDisplayErr err;
  CGDirectDisplayID ids[10];
  CGDisplayCount dspyCnt;
  err = CGGetActiveDisplayList(10, ids, &dspyCnt);
  
  if(pScreen < dspyCnt)
     return ids[pScreen];
  else
     return 0;
}

void MacKeyboardMouseSimulator::getScreenBounds(int pScreen, int &pX, int &pY, 
  int &pW, int &pH)
{
  CGDirectDisplayID dspyID;
  dspyID = convertUserScreenIdToMacDisplayId(pScreen); 
  
  CGRect r = CGDisplayBounds(dspyID);
  CGPoint p = r.origin;
  CGSize s = r.size;
  pX = p.x;
  pY = p.y;
  pW = s.width;
  pH = s.height;
}

void MacKeyboardMouseSimulator::mouseMoveFromTo(int pX0, int pY0, int pX1,
  int pY1)
{
  int stepsize = 50;
   
  int xSteps = (pX1 - pX0) / stepsize;
  int ySteps = (pY1 - pY0) / stepsize;

  int steps = std::max(std::max(abs(xSteps), abs(ySteps)), 1);

  int xStep = (pX1 - pX0) / steps;
  int yStep = (pY1 - pY0) / steps;

  for(int i = 0; i < steps; i++)
  {
    int xi = pX0 + i * xStep;
    int yi = pY0 + i * yStep;
    mouseMoveTo(xi, yi);
    // usleep(10000);
  }
  
  mouseMoveTo(pX1, pY1);
  // usleep(10000);
}

void MacKeyboardMouseSimulator::mouseMoveTo(int pX, int pY)
{
  CGPoint newloc;
  CGEventRef eventRef;
  
  newloc.x = pX;
  newloc.y = pY;
  
  eventRef = CGEventCreateMouseEvent(NULL, kCGEventMouseMoved, newloc,
    kCGMouseButtonCenter);
  CGEventSetType(eventRef, kCGEventMouseMoved);
  CGEventPost(kCGSessionEventTap, eventRef);
  CFRelease(eventRef);
}

void MacKeyboardMouseSimulator::mousePress(int pButton)
{
  CGPoint curloc;
  CGEventRef eventRef;
  
  CGEventRef ourEvent = CGEventCreate(NULL);
  curloc = CGEventGetLocation(ourEvent);
  CFRelease(ourEvent);
  
  CGEventType mouseEvent;
  if(pButton & MOUSE_BUTTON1_MASK)
    mouseEvent = kCGEventLeftMouseDown;
  else if(pButton & MOUSE_BUTTON2_MASK)
    mouseEvent = kCGEventOtherMouseDown;
  else if(pButton & MOUSE_BUTTON3_MASK)
    mouseEvent = kCGEventRightMouseDown;
  
  eventRef = CGEventCreateMouseEvent(NULL, mouseEvent, curloc,
    kCGMouseButtonCenter);
  
  CGEventSetType(eventRef, mouseEvent);
  CGEventPost(kCGSessionEventTap, eventRef);
  CFRelease(eventRef);
}

void MacKeyboardMouseSimulator::mouseRelease(int pButton)
{
  CGPoint curloc;
  CGEventRef eventRef;
  
  CGEventRef ourEvent = CGEventCreate(NULL);
  curloc = CGEventGetLocation(ourEvent);
  CFRelease(ourEvent);
  
  CGEventType mouseEvent;
  if(pButton & MOUSE_BUTTON1_MASK)
    mouseEvent = kCGEventLeftMouseUp;
  else if(pButton & MOUSE_BUTTON2_MASK)
    mouseEvent = kCGEventOtherMouseUp;
  else if(pButton & MOUSE_BUTTON3_MASK)
    mouseEvent = kCGEventRightMouseUp;
  
  eventRef = CGEventCreateMouseEvent(NULL, mouseEvent, curloc,
    kCGMouseButtonCenter);
  
  CGEventSetType(eventRef, mouseEvent);
  CGEventPost(kCGSessionEventTap, eventRef);
  CFRelease(eventRef);
}

void MacKeyboardMouseSimulator::debugList(std::list<unsigned int> pList)
{
  std::list<unsigned int>::iterator it;
  for(it = pList.begin(); it != pList.end(); ++it)
    std::cout << *it << std::endl;
}

void MacKeyboardMouseSimulator::setupKeyMap()
{  
  mKeyMap = new std::vector<Key>();
  
  mKeyMap->push_back(Key(VK_A, "a"));
  mKeyMap->push_back(Key(VK_B, "b"));
  mKeyMap->push_back(Key(VK_C, "c"));
  mKeyMap->push_back(Key(VK_D, "d"));
  mKeyMap->push_back(Key(VK_E, "e"));
  mKeyMap->push_back(Key(VK_F, "f"));
  mKeyMap->push_back(Key(VK_G, "g"));
  mKeyMap->push_back(Key(VK_H, "h"));
  mKeyMap->push_back(Key(VK_I, "i"));
  mKeyMap->push_back(Key(VK_J, "j"));
  mKeyMap->push_back(Key(VK_K, "k"));
  mKeyMap->push_back(Key(VK_L, "l"));
  mKeyMap->push_back(Key(VK_M, "m"));
  mKeyMap->push_back(Key(VK_N, "n"));
  mKeyMap->push_back(Key(VK_O, "o"));
  mKeyMap->push_back(Key(VK_P, "p"));
  mKeyMap->push_back(Key(VK_Q, "q"));
  mKeyMap->push_back(Key(VK_R, "r"));
  mKeyMap->push_back(Key(VK_S, "s"));
  mKeyMap->push_back(Key(VK_T, "t"));
  mKeyMap->push_back(Key(VK_U, "u"));
  mKeyMap->push_back(Key(VK_V, "v"));
  mKeyMap->push_back(Key(VK_W, "w"));
  mKeyMap->push_back(Key(VK_X, "x"));
  mKeyMap->push_back(Key(VK_Y, "y"));
  mKeyMap->push_back(Key(VK_Z, "z"));
  mKeyMap->push_back(Key(VK_BACK_QUOTE, "`"));
  mKeyMap->push_back(Key(VK_0, "0"));
  mKeyMap->push_back(Key(VK_1, "1"));
  mKeyMap->push_back(Key(VK_2, "2"));
  mKeyMap->push_back(Key(VK_3, "3"));
  mKeyMap->push_back(Key(VK_4, "4"));
  mKeyMap->push_back(Key(VK_5, "5"));
  mKeyMap->push_back(Key(VK_6, "6"));
  mKeyMap->push_back(Key(VK_7, "7"));
  mKeyMap->push_back(Key(VK_8, "8"));
  mKeyMap->push_back(Key(VK_9, "9"));
  mKeyMap->push_back(Key(VK_MINUS, "-"));
  mKeyMap->push_back(Key(VK_EQUALS, "="));
  mKeyMap->push_back(Key(VK_BACK_SPACE, "backspace"));
  mKeyMap->push_back(Key(VK_TAB, "tab"));
  mKeyMap->push_back(Key(VK_ENTER, "enter"));
  mKeyMap->push_back(Key(VK_RETURN, "return"));
  mKeyMap->push_back(Key(VK_OPEN_BRACKET, "("));
  mKeyMap->push_back(Key(VK_CLOSE_BRACKET, ")"));
  mKeyMap->push_back(Key(VK_BACK_SLASH, "\\"));
  mKeyMap->push_back(Key(VK_SEMICOLON, ";"));
  mKeyMap->push_back(Key(VK_QUOTE, "\""));
  mKeyMap->push_back(Key(VK_COMMA, ","));
  mKeyMap->push_back(Key(VK_PERIOD, "."));
  mKeyMap->push_back(Key(VK_SLASH, "/"));
  mKeyMap->push_back(Key(VK_SPACE, "space"));
  mKeyMap->push_back(Key(VK_ESCAPE, "ESC"));
  mKeyMap->push_back(Key(VK_UP, "up"));
  mKeyMap->push_back(Key(VK_RIGHT, "right"));
  mKeyMap->push_back(Key(VK_DOWN, "down"));
  mKeyMap->push_back(Key(VK_LEFT, "left"));
  mKeyMap->push_back(Key(VK_PAGE_UP, "page up"));
  mKeyMap->push_back(Key(VK_PAGE_DOWN, "page down"));
  mKeyMap->push_back(Key(VK_DELETE, "delete"));
  mKeyMap->push_back(Key(VK_END, "end"));
  mKeyMap->push_back(Key(VK_HOME, "home"));
  mKeyMap->push_back(Key(VK_INSERT, "insert"));
  mKeyMap->push_back(Key(VK_F1, "F1"));
  mKeyMap->push_back(Key(VK_F2, "F2"));
  mKeyMap->push_back(Key(VK_F3, "F3"));
  mKeyMap->push_back(Key(VK_F4, "F4"));
  mKeyMap->push_back(Key(VK_F5, "F5"));
  mKeyMap->push_back(Key(VK_F6, "F6"));
  mKeyMap->push_back(Key(VK_F7, "F7"));
  mKeyMap->push_back(Key(VK_F8, "F8"));
  mKeyMap->push_back(Key(VK_F9, "F9"));
  mKeyMap->push_back(Key(VK_F10, "F10"));
  mKeyMap->push_back(Key(VK_F11, "F11"));
  mKeyMap->push_back(Key(VK_F12, "F12"));
  mKeyMap->push_back(Key(VK_F13, "F13"));
  mKeyMap->push_back(Key(VK_F14, "F14"));
  mKeyMap->push_back(Key(VK_F15, "F15"));
  mKeyMap->push_back(Key(VK_CAPSLOCK, "CAPSLOCK"));
  mKeyMap->push_back(Key(VK_SHIFT, "shift"));
  mKeyMap->push_back(Key(VK_CONTROL, "ctrl"));
  mKeyMap->push_back(Key(VK_ALT, "alt"));
  mKeyMap->push_back(Key(VK_META, "cmd"));
}

} /* mkms */ 
