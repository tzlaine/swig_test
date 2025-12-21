level = { none = 0, trace = 1, log = 2, debug = 3 }
instrumentation_level = level.none

-- When lvl <= instrumentation_level, prints the concatenated string formed by
-- calling tostring() on obj and all following arguments.  Does nothing when
-- instrumentation_level < lvl.
function instrument(lvl, obj, ...)
   if instrumentation_level < lvl then
      return
   end
   local args = {...}
   local msg = tostring(obj)
   for i, v in ipairs(args) do
      msg = msg..tostring(v)
   end
   print(msg)
end

-- Returns a number converted to its truncated integer equivalent.  This must
-- be used when assigning to integer values bound in from the C++ side
-- (e.g. planet.max_population = x).  This function is provided because
-- math.tointeger is not robust -- it can produce a panic when converting
-- certain values.  The alternative is math.floor, but it does the wrong thing
-- when there is very small FP noise (e.g. the result of some computation is
-- should be 3, but the actual result is 2.999999996).
function to_integer(x)
   return math.floor(x + 0.0000001)
end
