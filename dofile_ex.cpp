extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

static int c_print(lua_State *L);

const char *Header = "Message";

int main(int argc, char *argv[]) {
    int ret, result, i;
    double sum;

    // Create the Lua interpreter
    lua_State *L = luaL_newstate();

    luaL_openlibs(L); // Load Lua libraries 

    /* Compile and load the Lua file given on the command line,
       but do not execute it (yet) */
    ret = luaL_loadfile(L, argv[1]);
    if (ret) {
        /* If something went wrong, error message is at the top of 
           the stack */
        std::cerr << "Couldn't load file: " << lua_tostring(L, -1) << std::endl;
        exit(1);
    }

    // Note: at this point, the file (as a compiled chunk) is at the top of the stack

    // Create a Lua table 
    lua_newtable(L);    // We will pass a table 

    /* lua_newtable always pushes the newly create table onto the top 
       of the stack  */

    // Push the key (name) and the value (Sally) onto the stack
    lua_pushstring(L, "name");
    lua_pushstring(L, "Sally");

    /* Now the stack looks like the follwoing (the numbers in the leftmost column 
       are relative indexes):
    
     -1  | Sally |  <---- Top of stack (TOS)
     -2  | name  |
     -3  | table |
     -4  | chunk |
          -------
    */
    
    /* lua_rawset pops the key and value off the stack and inserts them into the table
       identified by the give index (here it is -3). */
    lua_rawset(L, -3);      // Stores the pair in the table 

    // Note that now the table is at the top of the stack, which is perfect for a
    //  call to lua_setglobal, which sets at global variable to the value at the
    //  top of the stack. 
    lua_setglobal(L, "Info");    // At this point, the value at Info["name"] is "Sally"

    // Note: lua_setglobal pops the table from the stack, so it should be empty now.

    /* Now for something uber cool.  The following code is going to create a 
       userdata object, stuff a 1 in it (1 if the file descriptor for stdout),
       and create a C function that, when called, automatically gets the file
       descriptor.  A C function that automatically get such an "upvalue"
       when called is a "closure".
    */
    // Create a userdata.  A userdata is a Lua object that can host C/C++ data..
    int *ud = (int *) lua_newuserdata(L, sizeof(int));
    *ud = 1; // file descriptor 1 is standard out
    
    // Note that hte userdata is at the top of the stack

    // Now create a function (closure) that will automatically have the userdata upvalue
    //  when called
    lua_pushcclosure(L, c_print, 1);

    // Note that lua_push closure pops the userdata off the stack, create the closure,
    //  and then pushed it on the stack.  So, the closure is at the top of the stack.

    /* Give the function a name in the Lua interpreter so that it can be called.  Remember
       that lua_setglobal pops the value off the stack when it creates the global. */
    lua_setglobal(L, "c_print");

    /* Note that at this point, the only thing on the stack is the code chunk loaded
       at the beginning of this main.

       Now, run the script: Zero arguments, 1 return value, and 0 for no error function
                           _| ______________|                   |
                          |  |   _______________________________|    
                          |  |  |                                                      */
    result = lua_pcall(L, 0, 1, 0);
    if (result) {
      std::cerr << "Failed to run script: " << lua_tostring(L, -1) << std::endl;
      exit(1);
    }

    lua_pop(L, 1);  /* Take the returned value out of the stack */
    lua_close(L);   /* Adios, Lua */

    return 0;
}

// See the test.lua script to see how this closure is being called.
static int c_print(lua_State *L) {
  /* just to show that I can get the upvalue.  This is what makes this function
     a closure. */
  int *fd = (int *) lua_touserdata(L, lua_upvalueindex(1));
  // Now get the single argument passed by the call in the Lua script.
  const char *msg = (char *) lua_tostring(L, -1);
  // Now, write the argument using the file descriptor upvalue.
  write(*fd, "C++ : Writing passed argument: ", 31);
  write(*fd, msg, strlen(msg));
  write(*fd, "\n", 1);
  return 0;	
}
