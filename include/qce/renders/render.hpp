// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#ifdef WIN32
#  include <qce/renders/windows_output.hpp>
#  include <qce/renders/render_dx12.hpp>

namespace QCE {
    using GraphicsOutput = WinNtWindow;
}
#endif

