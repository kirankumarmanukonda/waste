// s3://your-bucket/animations/simpleAnimation.tsx
import React from 'react';
import { AbsoluteFill, useCurrentFrame } from 'remotion';
// Note: We are not importing animejs yet, just a basic Remotion component.

export default function MySimpleAnimation() {
  const frame = useCurrentFrame();
  return (
    <AbsoluteFill style={{ backgroundColor: 'lightblue', justifyContent: 'center', alignItems: 'center' }}>
      <h1 style={{ fontSize: 50 }}>Frame: {frame}</h1>
      <p>Hello from dynamic TSX!</p>
    </AbsoluteFill>
  );
}
