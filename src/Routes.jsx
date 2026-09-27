import React from "react";
import { BrowserRouter, Routes as RouterRoutes, Route } from "react-router-dom";
import EmbeddedAcademy from "pages/embedded-academy";
import CourseDetailPage from "pages/embedded-academy/CourseDetailPage";
import CourseModulePage from "pages/embedded-academy/CourseModulePage";

const Routes = () => {
  return (
    <BrowserRouter>
      <RouterRoutes>
        <Route path="/" element={<EmbeddedAcademy />} />
        <Route path="/courses/:slug" element={<CourseDetailPage />} />
        <Route path="/courses/:pathSlug/:moduleSlug" element={<CourseModulePage />} />
        <Route path="/courses/stm32" element={<CourseDetailPage />} />
        <Route path="/courses/freertos" element={<CourseDetailPage />} />
        <Route path="*" element={<EmbeddedAcademy />} />
      </RouterRoutes>
    </BrowserRouter>
  );
};

export default Routes;
