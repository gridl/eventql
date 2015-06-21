/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/json/jsonutil.h"

namespace fnord {
namespace json {

Option<String> JSONUtil::objectGetString(
    JSONObject::const_iterator begin,
    JSONObject::const_iterator end,
    const std::string& key) {
  auto iter = objectLookup(begin, end, key);

  if (iter != end && iter->type == JSON_STRING) {
    return Some(iter->data);
  } else {
    return None<String>();
  }
}

Option<uint64_t> JSONUtil::objectGetUInt64(
    JSONObject::const_iterator begin,
    JSONObject::const_iterator end,
    const std::string& key) {
  auto iter = objectLookup(begin, end, key);

  if (iter != end && (iter->type == JSON_STRING || iter->type == JSON_NUMBER)) {
    return Some<uint64_t>(std::stoul(iter->data));
  } else {
    return None<uint64_t>();
  }
}

Option<bool> JSONUtil::objectGetBool(
    JSONObject::const_iterator begin,
    JSONObject::const_iterator end,
    const std::string& key) {
  auto iter = objectLookup(begin, end, key);

  if (iter != end) {
    switch (iter->type) {

      case JSON_TRUE:
        return Some<bool>(true);

      case JSON_FALSE:
        return Some<bool>(false);

      case JSON_STRING:
        if (iter->data == "true" || iter->data == "TRUE")
          return Some<bool>(true);
        if (iter->data == "false" || iter->data == "FALSE")
          return Some<bool>(false);

        /* fallthrough */
    }
  }

  return None<bool>();
}

JSONObject::const_iterator JSONUtil::objectLookup(
    JSONObject::const_iterator begin,
    JSONObject::const_iterator end,
    const std::string& key) {
  if (begin == end) {
    return end;
  }

  if (begin->type != json::JSON_OBJECT_BEGIN) {
    RAISEF(kParseError, "expected JSON_OBJECT_BEGIN, got: $0", begin->type);
  }

  auto oend = begin + begin->size;
  if (oend > end) {
    return end;
  }

  for (++begin; begin < end;) {
    switch (begin->type) {
      case json::JSON_STRING:
        if (begin->data == key) {
          return ++begin;
        }

      default:
        break;
    }

    if (++begin == oend) {
      break;
    } else {
      begin += begin->size;
    }
  }

  return end;
}

size_t JSONUtil::arrayLength(
    JSONObject::const_iterator begin,
    JSONObject::const_iterator end) {
  if (begin == end) {
    return 0;
  }

  if (begin->type != json::JSON_ARRAY_BEGIN) {
    RAISEF(kParseError, "expected JSON_OBJECT_BEGIN, got: $0", begin->type);
  }

  auto aend = begin + begin->size - 1;
  if (aend > end) {
    RAISE(kIndexError);
  }

  size_t length = 0;
  for (++begin; begin < aend; begin += begin->size) {
    length++;
  }

  return length;
}

JSONObject::const_iterator JSONUtil::arrayLookup(
    JSONObject::const_iterator begin,
    JSONObject::const_iterator end,
    size_t index) {
  if (begin == end) {
    return end;
  }

  if (begin->type != json::JSON_ARRAY_BEGIN) {
    RAISEF(kParseError, "expected JSON_OBJECT_BEGIN, got: $0", begin->type);
  }

  auto aend = begin + begin->size - 1;
  if (aend > end || begin == aend) {
    RAISE(kIndexError);
  }

  size_t idx = 0;
  for (++begin; begin < aend; begin += begin->size) {
    if (idx++ == index) {
      return begin;
    }
  }

  RAISEF(kIndexError, "invalid index: $0", index);
  return end;
}

Option<String> JSONUtil::arrayGetString(
    JSONObject::const_iterator begin,
    JSONObject::const_iterator end,
    size_t index) {
  auto iter = arrayLookup(begin, end, index);

  if (iter != end && iter->type == JSON_STRING) {
    return Some(iter->data);
  } else {
    return None<String>();
  }
}

}
}

